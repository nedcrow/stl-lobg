// Fill out your copyright notice in the Description page of Project Settings.


#include "FairyPawn.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "MissileHub.h"
#include "FairyAIController.h"

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

	MissileHub = CreateDefaultSubobject<AMissileHub>(TEXT("MissileHub"));

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	PawnSensingComponent->bHearNoises = false;

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
}

// Called every frame
void AFairyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MissileHub->AddActorLocalRotation(FRotator(0, 0, 0));
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
	// HP bar UI ����
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

// �˸� �� �ǻ�ü �߻�
void AFairyPawn::ProcessSeenPawn(APawn * Pawn)
{
	// �߰� �˸�
	if (CurrentState == EFairyState::Idle) {
		SetCurrentState(EFairyState::Fight);
		//Set Blackboard Value
		AFairyAIController* AIC = GetController<AFairyAIController>();
		if (AIC)
		{
			AIC->SetEnemy(Pawn);
		}
	}


}

