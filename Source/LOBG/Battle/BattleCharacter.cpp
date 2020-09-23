// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABattleCharacter::ABattleCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Collision
	GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()), FRotator(0.f, -90.f, 0.f));

	// Camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector(0.f, GetCapsuleComponent()->GetScaledCapsuleRadius() + 10.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 10.f));
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// Movement
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

}

// Called when the game starts or when spawned
void ABattleCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABattleCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABattleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ABattleCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ABattleCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ABattleCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ABattleCharacter::Turn);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &ABattleCharacter::DoSprint);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &ABattleCharacter::UndoSprint);
}

// Move
void ABattleCharacter::MoveForward(float Value)
{
	if (Value == 0)
	{
		return;
	}

	//AddMovementInput(GetActorForwardVector(), Value);
	//AddMovementInput(FVector::ForwardVector, Value);
	FVector ControlForwardVector2D = GetControlRotation().Vector().GetSafeNormal2D();
	AddMovementInput(ControlForwardVector2D, Value);
}

void ABattleCharacter::MoveRight(float Value)
{
	if (Value == 0)
	{
		return;
	}

	//AddMovementInput(GetActorRightVector(), Value);
	//AddMovementInput(FVector::RightVector, Value);
	FVector ControlRightVector2D = GetControlRotation().RotateVector(FVector::RightVector).GetSafeNormal2D();
	AddMovementInput(ControlRightVector2D, Value);
}

void ABattleCharacter::LookUp(float Value)
{
	if (Value == 0)
	{
		return;
	}

	AddControllerPitchInput(Value);
}

void ABattleCharacter::Turn(float Value)
{
	if (Value == 0)
	{
		return;
	}

	AddControllerYawInput(Value);
}

// Sprint
void ABattleCharacter::DoSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	Server_SetMaxWalkSpeed(RunSpeed);
}

void ABattleCharacter::UndoSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	Server_SetMaxWalkSpeed(WalkSpeed);
}

void ABattleCharacter::Server_SetMaxWalkSpeed_Implementation(float NewSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}
