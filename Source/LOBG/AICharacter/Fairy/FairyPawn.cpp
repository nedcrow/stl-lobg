// Fill out your copyright notice in the Description page of Project Settings.


#include "FairyPawn.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "MeshesRing.h"
#include "MeshesRingComponent.h"
#include "FairyAIController.h"
#include "Engine/StaticMeshActor.h"
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

	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ETeam"), true);
}

// Called when the game starts or when spawned
void AFairyPawn::BeginPlay()
{
	Super::BeginPlay();	

	CurrentHP = MaxHP;

	SetCurrentState(EFairyState::Idle);

	if (PawnSensingComponent)
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AFairyPawn::ProcessSeenPawn);
	}

	//UE_LOG(LogClass, Warning, TEXT("Test_%d"), MissileHub->Meshes->Num());
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

float AFairyPawn::TakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (CurrentHP <= 0)
	{
		return 0.0f;
	}

	float TempHP = 0;
	// if (DamageCauser Tag == ��������) {
	//TempHP = CurrentHP + Damage;
	//}
	// else if (DamageCauser Tag == Bullet && EventInstigator ���� �ƴϸ�) {
	TempHP = CurrentHP - Damage;
	// ���� �� ���Ⱦ� �˸�
	// �ǰ� �ִϸ��̼�
	//}

	CurrentHP = FMath::Clamp(CurrentHP, 0.0f, 100.0f);

	if (CurrentHP <= 0)
	{
		// ���� �ִϸ��̼�
	}

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
	// HP UI ����
}

// Blackboard ������ State ����
void AFairyPawn::SetCurrentState(EFairyState NewState)
{
	CurrentState = NewState;
	AFairyAIController * AIC = GetController<AFairyAIController>();
	if (AIC)
	{
		AIC->SetCurrentState(NewState);
	}
}

// Sensing
void AFairyPawn::ProcessSeenPawn(APawn * Pawn)
{
	if (CurrentState == EFairyState::Idle) {
		SetCurrentState(EFairyState::Fight);
		//Set Blackboard Value
		AFairyAIController* AIC = GetController<AFairyAIController>();
		if (AIC) // CurrentTeam üũ �ʿ�
		{
			AIC->SetEnemy(Pawn);
			StartFireTo(Pawn->GetActorLocation());
		}
	}


}

// Fire
void AFairyPawn::StartFireTo(FVector TargetLocation)
{
	if (!bIsEndFire) {
		FVector StartLocation = MeshesRingComponent->InstanceBodies[MeshesRingComponent->GetInstanceCount() - 1]->GetUnrealWorldTransform().GetLocation();
		FRotator StartDirection = UKismetMathLibrary::GetDirectionUnitVector(StartLocation, TargetLocation).Rotation();
		MeshesRingComponent->RemoveOne();
		AStaticMeshActor* Missile = GetWorld()->SpawnActor<AStaticMeshActor>(MeshesRingComponent->GetStaticMesh()->GetClass(), StartLocation, StartDirection);
		// Missile �߻�
	}
}

void AFairyPawn::EndFire()
{
	bIsEndFire = true;
}



