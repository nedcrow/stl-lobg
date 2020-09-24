// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SceneComponent.h"
#include "BattlePC.h"
#include "../Weapon/BulletBase.h"

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

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(RootComponent);

	Tags.Add(TEXT("Player"));
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

void ABattleCharacter::OnFire()
{
	ABattlePC* PC = GetController<ABattlePC>();
	if (PC)
	{
		int32 ScreenSizeX;
		int32 ScreenSizeY;
		FVector CrosshairWorldLocation;
		FVector CrosshairWorldDirection;

		//화면 사이즈를 구해서
		PC->GetViewportSize(ScreenSizeX, ScreenSizeY);
		//화면 중앙의 2D좌표를 3D좌표로 구한다. 화면 중앙이라고 하면 crosshair가 있는 위치
		//위치와 방향을 구했다.
		PC->DeprojectScreenPositionToWorld(ScreenSizeX / 2, ScreenSizeY / 2, CrosshairWorldLocation, CrosshairWorldDirection);

		//카메라의 위치와 회전값 구하기
		FVector CameraLocation;
		FRotator CameraRotator;
		PC->GetPlayerViewPoint(CameraLocation, CameraRotator);

		//라인트레이스 인자들
		FVector StartVector = CameraLocation;
		FVector EndVector = StartVector + (CrosshairWorldDirection * 99999.f);
		
		Server_ProcessFire(StartVector, EndVector);
	}
}

void ABattleCharacter::Server_ProcessFire_Implementation(FVector StartLine, FVector EndLine)
{
	TArray<AActor*> IgnoreObj;
	FHitResult OutHit;

	bool Result = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), StartLine, EndLine,
		ObjectTypes, true, IgnoreObj, EDrawDebugTrace::ForDuration, OutHit, true, FLinearColor::Red, FLinearColor::Green, 1000.0f);

	if (Result && OutHit.GetActor() != nullptr)
	{
		UE_LOG(LogClass, Warning, TEXT("맞은놈은 %s"), *OutHit.GetActor()->GetName());

		//Muzzle에서 트레이스 Point까지의 회전값
		FRotator BulletRoation = (OutHit.ImpactPoint - Muzzle->GetComponentLocation()).Rotation();

		//Muzzle위치에서 조준점까지의 회전값을 가지고 총알 스폰
		ABulletBase* Bullet = GetWorld()->SpawnActor<ABulletBase>(BulletClass, Muzzle->GetComponentLocation(), BulletRoation);
		Bullet->SetDamageInfo(OutHit, GetController());
	}
	//라인트레이스가 돼도 OutHit에 할당이 안되면 실행되지 않게 합니다.
	else if (OutHit.GetActor() == nullptr)
	{
		UE_LOG(LogClass, Warning, TEXT("nullptr인 상황"));
		ABulletBase* Bullet = GetWorld()->SpawnActor<ABulletBase>(BulletClass, Muzzle->GetComponentLocation(), (EndLine - Muzzle->GetComponentLocation()).Rotation());
		Bullet->SetDamageInfo(OutHit, GetController());
	}
}

float ABattleCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UE_LOG(LogClass, Warning, TEXT("TakeDamage"));
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		UE_LOG(LogClass, Warning, TEXT("Dead"));
		Destroy();
	}
	return 0.0f;
}

void ABattleCharacter::StartIronsight()
{
}

void ABattleCharacter::StopIronsight()
{
}

void ABattleCharacter::C2S_SetIronsight_Implementation(bool State)
{
}

void ABattleCharacter::StartCrouch()
{
}

void ABattleCharacter::C2S_SetReload_Implementation(bool newState)
{
}
