// Fill out your copyright notice in the Description page of Project Settings.


#include "FairyPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "../../Battle/BattleCharacter.h"
#include "../../Battle/BattlePC.h"
#include "../../Weapon/BulletBase.h"
#include "../AIMinionChar.h"
#include "MeshesRingComponent.h"
#include "FairyAIController.h"
#include "../../UI/HUDBarSceneComponent.h"
#include "../../UI/HPBarWidgetBase.h"
#include "../../Weapon/BulletDamageType.h" 
#include "GameFramework/Actor.h"
#include "../../Battle/BattleGM.h"

//#include "../../Weapon/BulletBase.h"

// Sets default values
AFairyPawn::AFairyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetSphereRadius(64.f);
	RootComponent = SphereCollision;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(RootComponent);

	Head = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Head"));
	Head->SetupAttachment(Body);

	ActiveMeshesRingComp = CreateDefaultSubobject<UMeshesRingComponent>(TEXT("ActiveMeshesRingComponent"));
	ActiveMeshesRingComp->SetupAttachment(RootComponent);

	RestMeshesRingComp = CreateDefaultSubobject<UMeshesRingComponent>(TEXT("RestMeshesRingComponent"));
	RestMeshesRingComp->SetupAttachment(RootComponent);

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	PawnSensingComponent->bHearNoises = false;
	PawnSensingComponent->bOnlySensePlayers = false;
	PawnSensingComponent->SetPeripheralVisionAngle(178.f);

	HPBarHUD = CreateDefaultSubobject<UHUDBarSceneComponent>(TEXT("HPBarHUD"));
	HPBarHUD->SetupAttachment(RootComponent);

	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->SetupAttachment(HPBarHUD);
	Widget->SetRelativeRotation(FRotator(0,180,0));

	bIsCasting = false;
}

// Called when the game starts or when spawned
void AFairyPawn::BeginPlay()
{
	Super::BeginPlay();	

	NetMulticast_ResetTags(TEXT("None"));
	NetMulticast_InitHPBar(TeamColor);
	UpdateHPBar();

	SetCurrentState(EFairyState::Idle);

	if (PawnSensingComponent)
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AFairyPawn::ProcessSeenPawn);
	}

	// init arrays about missile reloading
	if (ActiveMeshesRingComp) {
		ReloadingPercentages.Init(-1, ActiveMeshesRingComp->MaxMeshCount);	
	}

	if (GetWorld()->IsServer()) {
		GetWorldTimerManager().SetTimer(BulletTimer, this, &AFairyPawn::Server_CallRotationRingComponent, 2.f, false);
	}
}

// Called every frame
void AFairyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AFairyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// 타워를 막타 팀으로 이전
float AFairyPawn::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (CurrentHP <= 0)
	{
		return 0.0f;
	}

	if (!GetWorld()->IsServer() || DamageCauser->ActorHasTag(TeamName))
	{
		return 0.f;
	}

	float TempHP = 0;
	TempHP = CurrentHP - Damage;

	// 피격 Sound
	if (HitSound) {
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(),
			HitSound,
			GetActorLocation()
		);
	}

	TempHP = FMath::Clamp(TempHP, 0.0f, MaxHP);
	if (CurrentHP != TempHP)
	{
		CurrentHP = TempHP;
		OnRepCurrentHP();
	}
	
	if (CurrentHP <= 0 && EventInstigator != NULL)
	{

		FName AttackerTeamName =	DamageCauser->ActorHasTag("Blue") ? "Blue" :
									DamageCauser->ActorHasTag("Red") ? "Red" : 
									"None";
		//NetMulticast_ResetTags(AttackerTeamName);


		// 죽인게 캐릭터인지 확인
		ABattlePC* AttackerPC = Cast<ABattlePC>(EventInstigator);
		if (AttackerPC)
		{
			ABattleCharacter* Pawn = Cast<ABattleCharacter>(AttackerPC->GetPawn());
			if (Pawn)
			{
				Pawn->Server_SetBooty(FairyMoney, FairyExp);
			}
		}

		if (bIsBoss && GetWorld() && CurrentState != EFairyState::Death) {
			ABattleGM* GM = Cast<ABattleGM>(GetWorld()->GetAuthGameMode());\
			if (GM && GetWorld()->IsServer())
			{
				GM->CountTower(TeamColor);
				GM->CallOpenResultTab(AttackerTeamName == "Blue" ? ETeamColor::Blue : ETeamColor::Red);
			}
		}
		NetMulticast_DeadthEffect();
		SetCurrentState(EFairyState::Death);
		

		Destroy();
	}
	
	return 0.0f;
}

void AFairyPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFairyPawn, MaxHP);
	DOREPLIFETIME(AFairyPawn, CurrentHP);
	DOREPLIFETIME(AFairyPawn, AttackPoint); 
}

void AFairyPawn::OnRepCurrentHP()
{
	UpdateHPBar();
}

void AFairyPawn::Server_CallRotationRingComponent_Implementation()
{
	if (ActiveMeshesRingComp && RestMeshesRingComp){
		ActiveMeshesRingComp->NetMulticast_StartRotateAround();
		RestMeshesRingComp->NetMulticast_StartRotateAround();
	}
	if (bIsBoss) {
		SpawnHeadEffect();
	}
}

// Blackboard 포함한 State 변경
void AFairyPawn::SetCurrentState(EFairyState NewState)
{
	CurrentState = NewState;
	AFairyAIController * AIC = GetController<AFairyAIController>();
	if (AIC)
	{
		AIC->SetCurrentState(NewState);
	}
}

// Sensing (StateChecking, TeamNameChecking, EnemySetting)
void AFairyPawn::ProcessSeenPawn(APawn * Pawn)
{
	if (CurrentState == EFairyState::Idle) {
		FName EnemyTeamName =	Pawn->ActorHasTag("Blue") ? "Blue" :
								Pawn->ActorHasTag("Red") ? "Red" :
								"None";
		if (EnemyTeamName != TeamName) {
			AFairyAIController* AIC = GetController<AFairyAIController>();
			if (AIC && AIC->CurrentEnermy != Pawn)
			{
				AIC->SetEnemy(Pawn);
				UE_LOG(LogTemp, Warning, TEXT("I found you [%s] :-)"), *EnemyTeamName.ToString());
			}
		}		
	}
}

// Fire
void AFairyPawn::StartFireTo(FVector TargetLocation)
{
	// ActiveMeshesRingComp 생성이 늦어진 경우 대비
	if (ReloadingPercentages.Num() < ActiveMeshesRingComp->MaxMeshCount) {
		ReloadingPercentages.Init(-1, ActiveMeshesRingComp->MaxMeshCount);
	}

	if(ActiveMeshesRingComp && ActiveMeshesRingComp->GetInstanceCount() > 0) {
		FTransform TempTransform;
		ActiveMeshesRingComp->GetInstanceTransform(0, TempTransform, true);
		FVector StartLocation = TempTransform.GetLocation();
		FRotator StartDirection = UKismetMathLibrary::GetDirectionUnitVector(StartLocation, TargetLocation).Rotation();

		Server_ProcessFire(StartLocation, StartDirection, TargetLocation);
	}
}

/* Tracing, RemoveIstanceMissile, Spawn missile, fire effect */
void AFairyPawn::Server_ProcessFire_Implementation(FVector StartLocation, FRotator StartDirection, FVector TargetLocation)
{
	TArray<AActor*> ActorToIgnore;

	FHitResult OutHit;

	bool Result = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		StartLocation,
		TargetLocation,
		ObjectTypes,
		true,
		ActorToIgnore,
		EDrawDebugTrace::None,
		OutHit,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f
	);
	

	// remove missile
	if (ActiveMeshesRingComp->GetInstanceCount() > 0) {
		NetMulticast_RemoveMissile();

		// reloading
		if (ActiveMeshesRingComp->GetInstanceCount() < ActiveMeshesRingComp->MaxMeshCount) {
			if (bIsCasting == false) {
				bIsCasting = true;

				NetMulticast_CallReloadAnimation();
			}
		}
	}

	// spawn missile
	ABulletBase* Bullet = GetWorld()->SpawnActor<ABulletBase>(BulletClass, StartLocation, StartDirection);
	if (Bullet)
	{
		Bullet->SetDamageInfo(GetController(), AttackPoint, AttackRadial, TeamName);
	}

	NetMulticast_FireEffect(StartLocation);
}

void AFairyPawn::NetMulticast_RemoveMissile_Implementation()
{
	ActiveMeshesRingComp->RemoveOne();
	ReloadingPercentages[CurrentMissileIndex] = 0;
	SetNextMissileIndex();
}

void AFairyPawn::SetNextMissileIndex()
{
	if (CurrentMissileIndex < ActiveMeshesRingComp->MaxMeshCount-1) { CurrentMissileIndex++; }
	else { CurrentMissileIndex = 0; }
}

void AFairyPawn::NetMulticast_CallReloadAnimation_Implementation()
{
	ReloadAnimation();
}

void AFairyPawn::ReloadAnimation()
{
	// About CallReloadAnimation loop
	bool Nextable = false; // 하나라도 로딩 중인 미사일이 있으면 true
	float SmoothPoint = 20; // frame per second
	float TimeUnit = ReloadingTime / (ReloadingTime * SmoothPoint);

	float MaxScale = 1;
	float StartScale = 0.1;

	// Set scale of each reloading meshes
	for (int i = 0; i < ReloadingPercentages.Num(); i++) {
		if (ReloadingPercentages[i] > -1 && ReloadingPercentages[i] < 1) {

			// update mesh scale 
			float Scale = ReloadingPercentages[i] == 0 ? StartScale : MaxScale * ReloadingPercentages[i];
			FTransform TempTransform = ActiveMeshesRingComp->SpawnTransforms[i];
			TempTransform.SetScale3D(FVector(Scale, Scale, Scale));
			RestMeshesRingComp->UpdateInstanceTransform(i, TempTransform, false, true, true);

			Nextable = true;
			
			ReloadingPercentages[i] += 1 / (ReloadingTime * SmoothPoint);
		}
		else if (ReloadingPercentages[i] > 1) {
			AddMissile(ActiveMeshesRingComp->SpawnTransforms[i]);

			// reset mesh scale
			FTransform TempTransform = ActiveMeshesRingComp->SpawnTransforms[i];
			TempTransform.SetScale3D(FVector(0, 0, 0));
			RestMeshesRingComp->UpdateInstanceTransform(i, TempTransform, false, true, true);

			// effect
			FTransform EffectTransform;
			ActiveMeshesRingComp->GetInstanceTransform(ActiveMeshesRingComp->GetInstanceCount()-1, EffectTransform, true);
			SpawnSpawnEffect(EffectTransform.GetLocation());

			ReloadingPercentages[i] = -1;
		}
	}

	if (Nextable) {
		GetWorldTimerManager().SetTimer(BulletTimer, this, &AFairyPawn::ReloadAnimation, TimeUnit, false);
	}
	else {
		bIsCasting = false;
		UE_LOG(LogTemp, Warning, TEXT("End reloading"));
	}
}

void AFairyPawn::AddMissile(FTransform NewTransform)
{
	ActiveMeshesRingComp->AddOne(NewTransform);
	UE_LOG(LogTemp, Warning, TEXT("AddMissile"));
}

void AFairyPawn::Repair()
{
	if (CurrentHP < MaxHP) {
		float TempHP = 0;
		TempHP = CurrentHP + RepairPerSec;
		CurrentHP = FMath::Clamp(TempHP, 0.0f, MaxHP);
		OnRepCurrentHP();
	}
}

//void AFairyPawn::NetMulticast_HeadEffect_Implementation()
//{
//	if (HeadEffect) {
//		UParticleSystemComponent* Particle = UGameplayStatics::SpawnEmitterAtLocation(
//			GetWorld(),
//			HeadEffect,
//			Head->GetRelativeLocation() + FVector(0, 0, 300.f)
//		);
//		FVector Temp = Head->GetRelativeLocation();
//		UE_LOG(LogTemp, Warning, TEXT("HeadEffect %f, %f, %f"), Temp.X, Temp.Y, Temp.Z);
//	}
//}

void AFairyPawn::SpawnHeadEffect()
{
	if (HeadEffect) {
		FVector Temp = GetActorLocation() + Head->GetRelativeLocation() + FVector(0, 0, 100.f);
		HeadEffectComponent = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HeadEffect,
			Temp
		);
	}	
}

void AFairyPawn::SpawnSpawnEffect(FVector SpawnLocation)
{
	if (SpawnEffect)
	{
		// UParticleSystemComponent* Particle = 
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			SpawnEffect,
			SpawnLocation
		);
	}
	if (ReloadSound) {
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(),
			ReloadSound,
			SpawnLocation
		);
	}
}

void AFairyPawn::NetMulticast_FireEffect_Implementation(FVector SpawnLocation)
{
	if (FireEffect)
	{
		// UParticleSystemComponent* Particle = 
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			FireEffect,
			SpawnLocation
		);
	}
}

void AFairyPawn::NetMulticast_DeadthEffect_Implementation()
{
	if (DeathEffect)
	{
		//UParticleSystemComponent* Particle = 
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			DeathEffect,
			GetActorLocation(),
			FRotator::ZeroRotator,
			FVector(2,2,2)
		);
	}
	if(HeadEffectComponent) HeadEffectComponent->DestroyComponent();
	if (DestroySound) {
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(),
			DestroySound,
			GetActorLocation()
		);
	}
}

void AFairyPawn::UpdateHPBar()
{
	UHPBarWidgetBase* HPWidget = Cast<UHPBarWidgetBase>(Widget->GetUserWidgetObject());
	if (HPWidget)
	{
		HPWidget->SetHPBar(CurrentHP / MaxHP);
	}
}

void AFairyPawn::NetMulticast_InitHPBar_Implementation(ETeamColor color)
{
	FLinearColor ColorRed = FLinearColor(1.f, 0, 0, 1.f);
	FLinearColor ColorBlue = FLinearColor(0, 0, 1.f, 1.f);

	//허드색 변경
	UHPBarWidgetBase* HPWidget = Cast<UHPBarWidgetBase>(Widget->GetUserWidgetObject());
	if (HPWidget)
	{
		if (color == ETeamColor::Red)
		{
			HPWidget->SetColorAndOpacity(ColorRed);
		}
		else if (color == ETeamColor::Blue)
		{
			HPWidget->SetColorAndOpacity(ColorBlue);
		}
	}
}

void AFairyPawn::NetMulticast_ResetTags_Implementation(const FName & TeamTag)
{
	if (Tags.Num() <= 1) {
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ETeamColor"), true);

		Tags.Emplace(TEXT("Tower"));
		if (enumPtr) Tags.Emplace(FName(enumPtr->GetNameStringByIndex((int32)TeamColor)));
		TeamName = FName(enumPtr->GetNameStringByIndex((int32)TeamColor));
	}
	else {
		Tags.Empty();
		Tags.Emplace(TEXT("Tower"));
		Tags.Emplace(TeamTag);
		TeamName = TeamTag;
	}

}





