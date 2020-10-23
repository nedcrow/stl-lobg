// Fill out your copyright notice in the Description page of Project Settings.


#include "FairyPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
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
		CurrentMissileIndex = ActiveMeshesRingComp->MaxMeshCount - 1;
		ReloadingPercentages.Init(-1, ActiveMeshesRingComp->MaxMeshCount);
		/*FirstLocalTransformArr.Init(FTransform(), ActiveMeshesRingComp->MaxMeshCount);
		FirstWorldTransformArr.Init(FTransform(), ActiveMeshesRingComp->MaxMeshCount);
		CurrentRestTransformArr.Init(FTransform(), ActiveMeshesRingComp->MaxMeshCount);
		for (int i=0; i< ActiveMeshesRingComp->MaxMeshCount; i++) {
			MissileIndexArr.Emplace(i);

			FTransform TempTransform;
			RestMeshesRingComp->GetInstanceTransform(i, TempTransform, true);
			CurrentRestTransformArr[i] = TempTransform;
		}		
		CurrentActiveTransformArr = ActiveMeshesRingComp->SpawnTransforms;*/
		
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
float AFairyPawn::TakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (CurrentHP <= 0)
	{
		return 0.0f;
	}

	float TempHP = 0;
	TempHP = CurrentHP - Damage;

	// 피격 애니메이션 추가 필요

	CurrentHP = FMath::Clamp(TempHP, 0.0f, MaxHP);
	OnRepCurrentHP();
	UE_LOG(LogTemp, Warning, TEXT("Ouch (%f)"), Damage );
	if (CurrentHP <= 0 && EventInstigator != NULL)
	{
		// 죽음 애니메이션(?) 추가 필요
		FName NewTeamName = "None";
		NewTeamName = GetTeamName(EventInstigator->GetPawn());
		UE_LOG(LogTemp, Warning, TEXT("Base is under attack (%s Team)"), *NewTeamName.ToString());
		NetMulticast_ResetTags(NewTeamName);
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
		FName EnemyTeamName = GetTeamName(Pawn);
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
	if(ActiveMeshesRingComp && ActiveMeshesRingComp->GetInstanceCount() > 0) {
		for (int i = ActiveMeshesRingComp->GetInstanceCount(); i > 0; i--) {
			if (ActiveMeshesRingComp->GetInstanceCount() > CurrentMissileIndex) {
				FTransform TempTransform;
				ActiveMeshesRingComp->GetInstanceTransform(CurrentMissileIndex, TempTransform, true);
				FVector StartLocation = TempTransform.GetLocation();
				FRotator StartDirection = UKismetMathLibrary::GetDirectionUnitVector(StartLocation, TargetLocation).Rotation();

				Server_ProcessFire(StartLocation, StartDirection, TargetLocation);
				break;
			}
			else {
				SetNextMissileIndex();
			}
		}
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


	if (ActiveMeshesRingComp->GetInstanceCount() > 0) {
		NetMulticast_RemoveCurrentMissile();

		if (ActiveMeshesRingComp->GetInstanceCount() < ActiveMeshesRingComp->MaxMeshCount) {
			if (bIsCasting == false) {
				bIsCasting = true;

				//NetMulticast_CallReloadAnimation();
				CallReloadAnimation();
			}
		}
	}
	

	ABulletBase* Bullet = GetWorld()->SpawnActor<ABulletBase>(BulletClass, StartLocation, StartDirection);
	if (Bullet)
	{
		Bullet->SetDamageInfo(GetController(), AttackPoint, AttackRadial, TeamName);
	}

	NetMulticast_FireEffect(StartLocation);
	SetNextMissileIndex();
}

void AFairyPawn::NetMulticast_RemoveCurrentMissile_Implementation()
{
	ActiveMeshesRingComp->RemoveOne(CurrentMissileIndex);
	ReloadingPercentages[CurrentMissileIndex] = 0;
}

void AFairyPawn::Server_AddMissile_Implementation(FTransform NewTransform)
{
	NetMulticast_AddMissile(NewTransform);
}

void AFairyPawn::NetMulticast_AddMissile_Implementation(FTransform NewTransform)
{
	ActiveMeshesRingComp->AddOne(NewTransform);
	UE_LOG(LogTemp,Warning,TEXT("AddMissile"));
}

int AFairyPawn::SetNextMissileIndex()
{
	if (CurrentMissileIndex > 0) { CurrentMissileIndex--; }
	else { CurrentMissileIndex = ActiveMeshesRingComp->MaxMeshCount - 1; }
	return CurrentMissileIndex;
}

void AFairyPawn::CallReloadAnimation()
{
	// About CallReloadAnimation loop
	bool Nextable = false;
	float SmoothPoint = 20; // frame per second
	float TimeUnit = ReloadingTime / (ReloadingTime * SmoothPoint);

	float MaxScale = 1;
	float StartScale = 0.1;
	
	// Set scale of each reloading meshes
	for (int i = 0; i < ReloadingPercentages.Num(); i++) {
		if (ReloadingPercentages[i] > -1 && ReloadingPercentages[i] < 1) {
			float Scale = ReloadingPercentages[i] == 0 ? StartScale : MaxScale * ReloadingPercentages[i];

			if (i < RestMeshesRingComp->GetInstanceCount()) {
				FTransform TempTransform;
				RestMeshesRingComp->GetInstanceTransform(i, TempTransform, false);
				TempTransform.SetScale3D(FVector(Scale, Scale, Scale));
				NetMulticast_UpdateReloadAnimation(i, TempTransform, false);

				Nextable = true;
			}
			ReloadingPercentages[i] += 1 / (ReloadingTime * SmoothPoint);
		}
		else if(ReloadingPercentages[i] > 1) {
			FTransform TempTransform;
			RestMeshesRingComp->GetInstanceTransform(i, TempTransform, false);
			TempTransform.SetScale3D(FVector(0.01, 0.01, 0.01));
			NetMulticast_UpdateReloadAnimation(i, TempTransform, true);
			NetMulticast_AddMissile(TempTransform);

			FTransform EffectTransform;
			RestMeshesRingComp->GetInstanceTransform(i, EffectTransform, true);
			NetMulticast_SpawnEffect(EffectTransform.GetLocation());
		}
	}

	if (Nextable) {
		GetWorldTimerManager().SetTimer(BulletTimer, this, &AFairyPawn::CallReloadAnimation, TimeUnit, false);
	}
	else {
		bIsCasting = false;
		UE_LOG(LogTemp, Warning, TEXT("End reloading"));
	}
}

void AFairyPawn::NetMulticast_CallReloadAnimation_Implementation()
{
	// About CallReloadAnimation loop
	bool Nextable = false;
	float SmoothPoint = 20; // frame per second
	float TimeUnit = ReloadingTime / (ReloadingTime * SmoothPoint);

	float MaxScale = 1;
	float StartScale = 0.1;

	// Set scale of each reloading meshes
	for (int i = 0; i < ReloadingPercentages.Num(); i++) {
		if (ReloadingPercentages[i] > -1 && ReloadingPercentages[i] < 1) {
			float Scale = ReloadingPercentages[i] == 0 ? StartScale : MaxScale * ReloadingPercentages[i];

			if (i < RestMeshesRingComp->GetInstanceCount()) {
				FTransform TempTransform;
				RestMeshesRingComp->GetInstanceTransform(i, TempTransform, false);
				TempTransform.SetScale3D(FVector(Scale, Scale, Scale));
				UpdateReloadAnimation(i, TempTransform, false);

				Nextable = true;
			}
			ReloadingPercentages[i] += 1 / (ReloadingTime * SmoothPoint);
		}
		else if (ReloadingPercentages[i] > 1) {
			FTransform TempTransform;
			RestMeshesRingComp->GetInstanceTransform(i, TempTransform, false);
			TempTransform.SetScale3D(FVector(0.01, 0.01, 0.01));
			UpdateReloadAnimation(i, TempTransform, true);
			//NetMulticast_AddMissile(TempTransform);
			Server_AddMissile(TempTransform);

			FTransform EffectTransform;
			RestMeshesRingComp->GetInstanceTransform(i, EffectTransform, true);
			//NetMulticast_SpawnEffect(EffectTransform.GetLocation());
		}
	}

	if (Nextable) {
		GetWorldTimerManager().SetTimer(BulletTimer, this, &AFairyPawn::NetMulticast_CallReloadAnimation, TimeUnit, false);
	}
	else {
		bIsCasting = false;
		UE_LOG(LogTemp, Warning, TEXT("End reloading"));
	}
}

void AFairyPawn::NetMulticast_UpdateReloadAnimation_Implementation(int Index, FTransform TargetTransform, bool End)
{
	RestMeshesRingComp->UpdateInstanceTransform(Index, TargetTransform, false, true, true);
	if (End) {
		ReloadingPercentages[Index] = -1;
		//UE_LOG(LogTemp, Warning, TEXT("Effect location : %d (%f, %f, %f)"), Index, TargetTransform.GetLocation().X, TargetTransform.GetLocation().Y, TargetTransform.GetLocation().Z);
	}
}

void AFairyPawn::UpdateReloadAnimation(int Index, FTransform TargetTransform, bool End)
{
	RestMeshesRingComp->UpdateInstanceTransform(Index, TargetTransform, false, true, true);
	if (End) {
		ReloadingPercentages[Index] = -1;
		//UE_LOG(LogTemp, Warning, TEXT("Effect location : %d (%f, %f, %f)"), Index, TargetTransform.GetLocation().X, TargetTransform.GetLocation().Y, TargetTransform.GetLocation().Z);
	}
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

void AFairyPawn::NetMulticast_SpawnEffect_Implementation(FVector SpawnLocation)
{
	if (SpawnEffect)
	{
		UParticleSystemComponent* Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			SpawnEffect,
			SpawnLocation
		);
	}
}

void AFairyPawn::NetMulticast_FireEffect_Implementation(FVector SpawnLocation)
{
	if (FireEffect)
	{
		UParticleSystemComponent* Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			FireEffect,
			SpawnLocation
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

FName AFairyPawn::GetTeamName(APawn * Pawn)
{
	FName NewTeamName = "None";
	if (Pawn->ActorHasTag(TEXT("Player"))) {
		NewTeamName = Cast<ABattleCharacter>(Pawn)->TeamName;
	}
	else if(Pawn->ActorHasTag(TEXT("Minion"))) {
		NewTeamName = Cast<AAIMinionChar>(Pawn)->TeamName;
	}
	else if (Pawn->ActorHasTag(TEXT("Tower"))) {
		NewTeamName = Cast<AFairyPawn>(Pawn)->TeamName;
	}
	return NewTeamName;
}





