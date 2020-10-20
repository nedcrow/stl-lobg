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

	HPBarHUD = CreateDefaultSubobject<UHUDBarSceneComponent>(TEXT("HPBarHUD"));
	HPBarHUD->SetupAttachment(RootComponent);

	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->SetupAttachment(HPBarHUD);
	Widget->SetRelativeRotation(FRotator(0,180,0));

	bIsCasting=false;
}

// Called when the game starts or when spawned
void AFairyPawn::BeginPlay()
{
	Super::BeginPlay();	

	NetMulticast_ResetTags(TEXT("None"));
	UpdateHPBar();

	SetCurrentState(EFairyState::Idle);

	if (PawnSensingComponent)
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AFairyPawn::ProcessSeenPawn);
	}

	if(ActiveMeshesRingComp) CurrentBulletCount = ActiveMeshesRingComp->MaxMeshCount;
	for (; CurrentBulletCount > 0; CurrentBulletCount--) {
		ReloadingPercentages.Add(0);
	}
	
	SetRingComponentRotation();
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

void AFairyPawn::SetRingComponentRotation()
{
	/*ActiveMeshesRingComp->SetRelativeRotation(tempRot);
	RestMeshesRingComp->SetRelativeRotation(tempRot);*/
	/*ActiveMeshesRingComp->NetMulticast_RotateAround(TempRot);
	RestMeshesRingComp->NetMulticast_RotateAround(TempRot);*/

	if (UGameplayStatics::GetGameMode(GetWorld())) {
		FRotator TempRot = ActiveMeshesRingComp->GetRelativeRotation();
		UE_LOG(LogTemp, Warning, TEXT("TempRotator:: %f, %f, %f"), TempRot.Roll, TempRot.Yaw, TempRot.Pitch);
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
	if(ActiveMeshesRingComp) {
		int currentInstanceCount = ActiveMeshesRingComp->GetInstanceCount();
		if (currentInstanceCount > 0) {
			FTransform TempTransform;
			ActiveMeshesRingComp->GetInstanceTransform(currentInstanceCount - 1, TempTransform, true);
			FVector StartLocation = TempTransform.GetLocation();
			FRotator StartDirection = UKismetMathLibrary::GetDirectionUnitVector(StartLocation, TargetLocation).Rotation();

			Server_ProcessFire(StartLocation, StartDirection, TargetLocation);
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

	ActiveMeshesRingComp->NetMulticast_RemoveOne();
	CurrentBulletCount--;
	Server_CallReload();

	ABulletBase* Bullet = GetWorld()->SpawnActor<ABulletBase>(BulletClass, StartLocation, StartDirection);
	if (Bullet)
	{
		Bullet->SetDamageInfo(GetController(), AttackPoint, 200, TeamName);
	}

	NetMulticast_FireEffect(StartLocation);
	TempEffectLocation = StartLocation;
}

void AFairyPawn::Server_CallReload_Implementation()
{
	// 총알 수량 변하면 실행
	// 총알이 max 미만이고, casting 중이 아니면 실행
	if (CurrentBulletCount < ActiveMeshesRingComp->MaxMeshCount) {
		if (bIsCasting == false) {
			bIsCasting = true;
			ReloadingPercentage = 0;
			GetWorldTimerManager().SetTimer(BulletTimer, this, &AFairyPawn::Reload, 12.f, false);
			ReloadAnimation();
		}
	}
}

void AFairyPawn::Reload()
{
	ActiveMeshesRingComp->NetMulticast_AddOne();
	CurrentBulletCount++;
	bIsCasting = false;
	Server_CallReload();
}

void AFairyPawn::ReloadAnimation()
{
	if (ReloadingPercentage < 1) {
		float MaxScale = 1;
		float StartScale = 0.1;
		float SmoothPoint = 20; // frame per second
		float TimeUnit = ReloadingTime / (ReloadingTime * SmoothPoint);
		float Scale = ReloadingPercentage == 0? StartScale : MaxScale * ReloadingPercentage;

		if (ActiveMeshesRingComp->GetInstanceCount() < RestMeshesRingComp->GetInstanceCount()) {
			int Index = ActiveMeshesRingComp->GetInstanceCount();
			FTransform TempTransform = RestMeshesRingComp->SpawnTransforms[Index];
			TempTransform.SetScale3D(FVector(Scale, Scale, Scale));
			NetMulticast_EndReloadAnimation(Index, TempTransform);
			
			GetWorldTimerManager().SetTimer(BulletTimer, this, &AFairyPawn::ReloadAnimation, TimeUnit, false);
		}
		ReloadingPercentage += 1 / (ReloadingTime * SmoothPoint);
	} else {
		int Index = ActiveMeshesRingComp->GetInstanceCount();
		FTransform TempTransform = RestMeshesRingComp->SpawnTransforms[Index];
		TempTransform.SetScale3D(FVector(0.01, 0.01, 0.01));
		NetMulticast_EndReloadAnimation(Index, TempTransform);

		Reload();
		NetMulticast_SpawnEffect(TempEffectLocation);
	}
}

void AFairyPawn::NetMulticast_EndReloadAnimation_Implementation(int Index, FTransform TargetTransform)
{
	RestMeshesRingComp->UpdateInstanceTransform(Index, TargetTransform, false, true, true);
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

void AFairyPawn::NetMulticast_ResetTags_Implementation(const FName & TeamTag)
{
	if (Tags.Num() <= 1) {
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ETeamColor"), true);

		Tags.Add(TEXT("Tower"));
		if (enumPtr) Tags.Add(FName(enumPtr->GetNameStringByIndex((int32)TeamColor)));
		TeamName = FName(enumPtr->GetNameStringByIndex((int32)TeamColor));
		UE_LOG(LogTemp, Warning, TEXT("AAA:%s"), *TeamName.ToString());
	}
	else {
		Tags.Empty();
		Tags.Add(TEXT("Tower"));
		Tags.Add(TeamTag);
		TeamName = TeamTag;
		UE_LOG(LogTemp, Warning, TEXT("BBB:%s"), *TeamName.ToString());
	}

}

FName AFairyPawn::GetTeamName(APawn * Pawn)
{
	FName NewTeamName = "None";
	if (Pawn->ActorHasTag(TEXT("Player"))) {
		NewTeamName = Cast<ABattleCharacter>(Pawn)->TeamName;
	}
	else if(Pawn->ActorHasTag(TEXT("Minion"))) {
		NewTeamName = Cast<ABattleCharacter>(Pawn)->TeamName;
	}
	else if (Pawn->ActorHasTag(TEXT("Tower"))) {
		NewTeamName = Cast<AFairyPawn>(Pawn)->TeamName;
	}
	return NewTeamName;
}



