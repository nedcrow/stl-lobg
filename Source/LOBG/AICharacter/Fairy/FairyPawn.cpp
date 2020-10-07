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

	MeshesRingComponent = CreateDefaultSubobject<UMeshesRingComponent>(TEXT("MeshesRingComponent"));
	MeshesRingComponent->SetupAttachment(Body);

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	PawnSensingComponent->bHearNoises = false;

	HPBarHUD = CreateDefaultSubobject<UHUDBarSceneComponent>(TEXT("HPBarHUD"));
	HPBarHUD->SetupAttachment(RootComponent);

	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->SetupAttachment(HPBarHUD);

	Widget->SetRelativeRotation(FRotator(0,180,0));

	CurrentHP = MaxHP = 100;
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
		bIsEndFire = true;
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
	//if (DamageEvent.IsOfType(FPointDamageEvent::ClassID)) {
		float TempHP = 0;
		TempHP = CurrentHP - Damage;
		UE_LOG(LogTemp, Warning, TEXT("누가 나 때림! (HP : %f)"), TempHP);
		// 피격 애니메이션

		CurrentHP = FMath::Clamp(TempHP, 0.0f, 100.0f);
		UpdateHPBar();

		if (CurrentHP <= 0 && EventInstigator != NULL)
		{
			// 죽음 애니메이션(?)
			FName NewTeamName = "None";
			NewTeamName = GetTeamName(EventInstigator->GetPawn());
			UE_LOG(LogTemp, Warning, TEXT("from_(%s Team)"), *NewTeamName.ToString());
			NetMulticast_ResetTags(NewTeamName);
		}
	//}

	
	return 0.0f;
}

void AFairyPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFairyPawn, MaxHP);
	DOREPLIFETIME(AFairyPawn, CurrentHP);
}

void AFairyPawn::OnRepCurrentHP()
{
	// HP UI 변경
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
			//SetCurrentState(EFairyState::Fight);
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
	//if (bIsEndFire) {
		bIsEndFire = false;
		int currentInstanceCount = MeshesRingComponent->GetInstanceCount();
		if (currentInstanceCount > 0) {
			FTransform TempTransform;
			MeshesRingComponent->GetInstanceTransform(currentInstanceCount - 1, TempTransform, true);
			FVector StartLocation = TempTransform.GetLocation();
			FRotator StartDirection = UKismetMathLibrary::GetDirectionUnitVector(StartLocation, TargetLocation).Rotation();
			MeshesRingComponent->RemoveOne();
			UE_LOG(LogTemp, Warning, TEXT("Fire!"));
			ABulletBase* Bullet = GetWorld()->SpawnActor<ABulletBase>(BulletClass, StartLocation, StartDirection);
			// Missile 발사
			//Server_ProcessFire(StartLocation,TargetLocation);
		}
	//}
}

void AFairyPawn::Server_ProcessFire_Implementation(FVector StartLocation, FVector TargetLocation)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> Objects;
	Objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	Objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	Objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));

	TArray<AActor*> ActorToIgnore;

	FHitResult OutHit;

	bool Result = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		StartLocation,
		TargetLocation,
		Objects,
		true,
		ActorToIgnore,
		EDrawDebugTrace::None,
		OutHit,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f
	);

	
	//all client spawn Hit effect and Decal
	//Multicast_SpawnHitEffectAndDecal(OutHit);

	//Point Damage
	UGameplayStatics::ApplyPointDamage(OutHit.GetActor(), //맞은놈
		AttackPoint,	//데미지
		-OutHit.ImpactNormal,	//데미지 방향
		OutHit,	//데미지 충돌 정보
		GetController(),	//때린 플레이어
		this,	//때린놈
		UBulletDamageType::StaticClass() //데미지 타입
	);

	MakeNoise(1.0f, this, OutHit.ImpactPoint);
	
}

void AFairyPawn::EndFire()
{
	bIsEndFire = true;
}

void AFairyPawn::Reload()
{
	// 총알이 몇 개 미만일 때,
	if (MeshesRingComponent->GetInstanceCount() < MeshesRingComponent->MeshCount) {
		MeshesRingComponent->AddOne();
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



