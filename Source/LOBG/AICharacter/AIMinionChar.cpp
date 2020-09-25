// Fill out your copyright notice in the Description page of Project Settings.


#include "AIMinionChar.h"

#include "MinionAIC.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AAIMinionChar::AAIMinionChar()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Collision
	GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()), FRotator(0.f, -90.f, 0.f));

	// Movement
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// ETC
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

}

// Called when the game starts or when spawned
void AAIMinionChar::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAIMinionChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAIMinionChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAIMinionChar::SetState(EMinioonState NewState)
{
	CurrentState = NewState;

	AMinionAIC* MinionAIC = GetController<AMinionAIC>();
	if (MinionAIC)
	{
		MinionAIC->SetValueState(NewState);
	}
}

