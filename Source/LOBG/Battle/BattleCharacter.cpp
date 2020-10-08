// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "BattlePC.h"
#include "BattleGM.h"
#include "BattlePS.h"
#include "BattleWidgetBase.h"
#include "CharacterAnimInstance.h"
#include "../Weapon/BulletBase.h"
#include "../ReSpawn/ReSpawn.h"
#include "Kismet/GameplayStatics.h"
#include "../LOBGGameInstance.h"
#include "../Weapon/WeaponComponent.h"
#include "Components/WidgetComponent.h"
#include "../UI/HPBarWidgetBase.h"
#include "../UI/HUDBarSceneComponent.h"


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

	Weapon = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));

	HPBarHUD = CreateDefaultSubobject<UHUDBarSceneComponent>(TEXT("HPBarHUD"));
	HPBarHUD->SetupAttachment(RootComponent);
	HPBarHUD->SetRelativeLocation(FVector(0, 0, 110));

	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->SetupAttachment(HPBarHUD);
	Widget->SetRelativeRotation(FRotator(0, 180.f, 0));
	//Widget->SetIsReplicated(true);

	Tags.Add(TEXT("Player"));
}

// Called when the game starts or when spawned
void ABattleCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHP = MaxHP;
	CurrentState = EBattleCharacterState::Normal;

	//UI초기화 및 유무 확인
	OnRep_CurrentHP();
	ABattlePS* PS = GetPlayerState<ABattlePS>();
	if (PS)
	{
		PS->OnRep_Exp();
		PS->OnRep_Money();
	}

	UE_LOG(LogClass, Warning, TEXT("Character BeginPlay"));
	if (IsLocallyControlled())
	{
		//Widget->SetVisibility(false);
	}
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

	PlayerInputComponent->BindAction(TEXT("Ironsight"), IE_Pressed, this, &ABattleCharacter::StartIronsight);
	PlayerInputComponent->BindAction(TEXT("Ironsight"), IE_Released, this, &ABattleCharacter::StopIronsight);

	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ABattleCharacter::StartCrouch);

	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &ABattleCharacter::StartReload);

	PlayerInputComponent->BindAction(TEXT("LeanLeft"), IE_Pressed, this, &ABattleCharacter::StartLeanLeft);
	PlayerInputComponent->BindAction(TEXT("LeanLeft"), IE_Released, this, &ABattleCharacter::StopLeanLeft);
	PlayerInputComponent->BindAction(TEXT("LeanRight"), IE_Pressed, this, &ABattleCharacter::StartLeanRight);
	PlayerInputComponent->BindAction(TEXT("LeanRight"), IE_Released, this, &ABattleCharacter::StopLeanRight);
}

void ABattleCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABattleCharacter, bIsIronsight);
	DOREPLIFETIME(ABattleCharacter, bIsFire);
	DOREPLIFETIME(ABattleCharacter, bIsReload);
	DOREPLIFETIME(ABattleCharacter, CurrentHP);
	DOREPLIFETIME(ABattleCharacter, MaxHP);
	DOREPLIFETIME(ABattleCharacter, CurrentState);
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

void ABattleCharacter::StartFire()
{
	bIsFire = true;
	OnFire();
}

void ABattleCharacter::StopFire()
{
	bIsFire = false;
}

void ABattleCharacter::OnFire()
{
	if (!bIsFire) return;

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
	GetWorldTimerManager().SetTimer(BulletTimer, this, &ABattleCharacter::OnFire, 0.12f, false);
}

void ABattleCharacter::Server_ProcessFire_Implementation(FVector StartLine, FVector EndLine)
{
	TArray<AActor*> IgnoreObj;
	FHitResult OutHit;

	bool Result = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), StartLine, EndLine,
		ObjectTypes, true, IgnoreObj, EDrawDebugTrace::None, OutHit, true, FLinearColor::Red, FLinearColor::Green, 1000.0f);

	if (Result && OutHit.GetActor() != nullptr)
	{
		//Muzzle에서 트레이스 Point까지의 회전값
		FRotator BulletRoation = (OutHit.ImpactPoint - Weapon->GetSocketLocation(TEXT("Muzzle"))).Rotation();

		//Muzzle위치에서 조준점까지의 회전값을 가지고 총알 스폰
		ABulletBase* Bullet = GetWorld()->SpawnActor<ABulletBase>(BulletClass, Weapon->GetSocketLocation(TEXT("Muzzle")), BulletRoation);
			
		if (Bullet)
		{
			Bullet->SetDamageInfo(OutHit, GetController());
			Bullet->TeamName = TeamName;
		}
	}
	//액터가 할당되지 않은 경우 : 하늘에 쐈을 때 = EndLine끝을 향해 쏜다.
	//하늘에 쏴도 도중에 아무 액터나 맞을 때를 대비해서 OutHit를 전해준다.
	else if (OutHit.GetActor() == nullptr)
	{
		ABulletBase* Bullet = GetWorld()->SpawnActor<ABulletBase>(BulletClass, Weapon->GetSocketLocation(TEXT("Muzzle")), (EndLine - Weapon->GetSocketLocation(TEXT("Muzzle"))).Rotation());
			
		if (Bullet)
		{
			Bullet->SetDamageInfo(OutHit, GetController());
			Bullet->TeamName = TeamName;
		}
	}
}

float ABattleCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (CurrentHP <= 0) return 0.f;

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{

		//Destroy();

		// 피격 시 HitAnimation
		NetMulticast_StartHitMontage(FMath::RandRange(1, 4));

		//네트워크상에서 CurrentHP동기화를 한번한 하기 위한 장치
		float TempHP = CurrentHP;

		FPointDamageEvent* PointDamageEvent = (FPointDamageEvent*)(&DamageEvent);
		if (PointDamageEvent->HitInfo.BoneName.Compare(TEXT("head")) == 0)
		{
			//총, 총알 타입에 따라 헤드샷을 맞았을 때 다른 데미지 들어간다

			TempHP = 0;
		}
		else
		{
			TempHP -= DamageAmount;
		}

		TempHP = FMath::Clamp(TempHP, 0.f, 100.f);

		CurrentHP = TempHP;
		//서버도 실행되게
		OnRep_CurrentHP();
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
	}
	else if (DamageEvent.IsOfType(FDamageEvent::ClassID))
	{
	}

	//죽었을때
	if (CurrentHP <= 0)
	{
		NetMulticast_StartDeath(FMath::RandRange(1, 3));
		CurrentState = EBattleCharacterState::Dead;
		FTimerHandle DeadTimer;
		GetWorldTimerManager().SetTimer(DeadTimer, this, &ABattleCharacter::Server_CallReSpawnToGM, 5.0f, false);
	}

	return 0.0f;
}

void ABattleCharacter::OnRep_CurrentHP()
{
	ABattlePC* PC = Cast<ABattlePC>(GetController());
	if (PC && PC->IsLocalController())
	{
		if (PC->BattleWidgetObject)
		{
			PC->BattleWidgetObject->SetHPBar(CurrentHP / MaxHP);
		}
	}

	//HPHUD업데이트
	UpdateHPBar();
}

void ABattleCharacter::Server_SetIronsight_Implementation(bool State)
{
	bIsIronsight = State;
}

void ABattleCharacter::StartIronsight()
{
	bIsIronsight = true;
	Server_SetIronsight(true);
}

void ABattleCharacter::StopIronsight()
{
	bIsIronsight = false;
	Server_SetIronsight(false);
}

void ABattleCharacter::StartCrouch()
{
	if (CanCrouch())
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}

void ABattleCharacter::Server_SetLeanLeft_Implementation(bool State)
{
	bIsLeanLeft = State;
}

void ABattleCharacter::Server_SetLeanRight_Implementation(bool State)
{
	bIsLeanRight = State;
}

void ABattleCharacter::StartLeanLeft()
{
	bIsLeanLeft = true;
	Server_SetLeanLeft(true);
}

void ABattleCharacter::StopLeanLeft()
{
	bIsLeanLeft = false;
	Server_SetLeanLeft(false);
}

void ABattleCharacter::StartLeanRight()
{
	bIsLeanRight = true;
	Server_SetLeanRight(true);
}

void ABattleCharacter::StopLeanRight()
{
	bIsLeanRight = false;
	Server_SetLeanRight(false);
}

void ABattleCharacter::NetMulticast_StartDeath_Implementation(int Index)
{
	if (DeathMontage) {
		FString DeathSectionName = FString::Printf(TEXT("Death%d"), Index);
		PlayAnimMontage(DeathMontage, 1.f, FName(DeathSectionName));
		DeathSetting();
	}
}

void ABattleCharacter::Server_SetReload_Implementation(bool NewState)
{
	bIsReload = NewState;
	UCharacterAnimInstance* AnimInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	
}

void ABattleCharacter::StartReload()
{
	
	bIsReload = true;
	Server_SetReload(true);
	
}

void ABattleCharacter::NetMulticast_StartHitMontage_Implementation(int Number)
{
	if (HitActionMontage) {
		FString HitSectionName = FString::Printf(TEXT("Hit%d"), Number);
		PlayAnimMontage(HitActionMontage, 1.f, FName(HitSectionName));
	}
}

void ABattleCharacter::Server_CallReSpawnToGM_Implementation()
{
	ABattleGM* GM = Cast<ABattleGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		GM->CallReSpawn(this);
		Destroy();
	}
}

void ABattleCharacter::NetMulticast_ReSpawnUI_Implementation()
{
	OnRep_CurrentHP();
	ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
	if (PS)
	{
		PS->OnRep_Exp();
		PS->OnRep_Money();
	}
}

void ABattleCharacter::DeathSetting()
{
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	//GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));
	DisableInput(Cast<APlayerController>(GetController()));
}

void ABattleCharacter::Server_SetBooty_Implementation(int Money, float Exp)
{
	ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
	if (PS)
	{
		PS->PlayerMoney += Money;
		PS->OnRep_Money();

		//PS->PlayerExp += Exp;
		//PS->SetExp(Exp);
		PS->OnRep_Exp();
	}
}

void ABattleCharacter::SetBooty(int Money, float Exp)
{
	ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
	if (PS)
	{
		PS->PlayerMoney += Money;
		PS->OnRep_Money();

		PS->PlayerExp += Exp;
		PS->NewExp += Exp;
		PS->OnRep_Exp();
	}
}

void ABattleCharacter::NetMulticast_AddTag_Implementation(const FName & PlayerTag)
{
	Tags.Add(PlayerTag);
	TeamName = PlayerTag;
}

void ABattleCharacter::InitHPBarWithEnum(ETeamColor color)
{
	UHPBarWidgetBase* HPWidget = Cast<UHPBarWidgetBase>(Widget->GetUserWidgetObject());
	if (HPWidget)
	{
		
		if (color == ETeamColor::Red)
		{
			HPWidget->SetColorAndOpacity(FLinearColor(1.f, 0, 0, 1.f));
		}
		else if (color == ETeamColor::Blue)
		{
			HPWidget->SetColorAndOpacity(FLinearColor(0, 0, 1.f, 1.f));
		}
	}
}

void ABattleCharacter::UpdateHPBar()
{
	UHPBarWidgetBase* HPWidget = Cast<UHPBarWidgetBase>(Widget->GetUserWidgetObject());
	if (HPWidget)
	{
		HPWidget->SetHPBar(CurrentHP / MaxHP);
	}
}

void ABattleCharacter::SetHUDVisible()
{
	if (Widget)
	{
		if (IsLocallyControlled())
		{
			Widget->SetVisibility(false);
		}
	}
}

