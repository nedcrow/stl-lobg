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
	PrimaryActorTick.bCanEverTick = false;

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
		//WalkSpeed = PS->PlayerSpeed;
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

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ABattleCharacter::StartFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &ABattleCharacter::StopFire);

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
	DOREPLIFETIME(ABattleCharacter, WalkSpeed);
	DOREPLIFETIME(ABattleCharacter, RunSpeed);
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
	RunSpeed = WalkSpeed * 1.25f;
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

	GetWorldTimerManager().SetTimer(BulletTimer, this, &ABattleCharacter::OnFire, 0.2f, false);
}

void ABattleCharacter::Server_ProcessFire_Implementation(FVector StartLine, FVector EndLine)
{
	TArray<AActor*> IgnoreObj;
	FHitResult OutHit;

	bool Result = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), StartLine, EndLine,
		ObjectTypes, true, IgnoreObj, EDrawDebugTrace::None, OutHit, true, FLinearColor::Red, FLinearColor::Green, 1000.0f);



	
	// 액터가 할당되지 않은 경우 : 하늘에 쐈을 때 = EndLine끝을 향해 쏜다.
	// 하늘에 쏴도 도중에 아무 액터나 맞을 때를 대비해서 OutHit를 전해준다.	
	FVector EndVector(EndLine);

	// 히트 결과가 있을 경우 해당 지점으로 쏜다.
	if (Result)
	{
		EndVector = OutHit.ImpactPoint;
	}
	
	//Muzzle에서 트레이스 Point까지의 회전값
	FRotator BulletRoation = (EndVector - Weapon->GetSocketLocation(TEXT("Muzzle"))).Rotation();



	// 총알 확산.
	float FireAngle = RandFireAngle;
	if (bIsIronsight)
	{
		//조준 하면 확산각 하락.
		FireAngle = RandFireAngle / 4.f;
	}

	if (GetCharacterMovement()->IsFalling())
	{
		// 점프한 상태면 확산각 증가. 
		FireAngle *= 2.f;
	}

	BulletRoation.Yaw += FMath::FRandRange(-FireAngle, FireAngle);
	BulletRoation.Pitch += FMath::FRandRange(-FireAngle, FireAngle);



	// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	const FVector SpawnLocation = (Weapon != nullptr) ? Weapon->GetSocketLocation(TEXT("Muzzle")) : GetActorLocation() + BulletRoation.RotateVector(FVector(100.0f, 0.0f, 10.0f));




	// 모든 클라이언트에서 총알 스폰.
	NetMulticast_ProcessFire(SpawnLocation, BulletRoation);




}

void ABattleCharacter::NetMulticast_ProcessFire_Implementation(FVector SpawnLocation, FRotator SpawnRotation)
{
	//Set Spawn Collision Handling Override
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 총알 스폰
	ABulletBase* Bullet = GetWorld()->SpawnActor<ABulletBase>(BulletClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

	// 총알 초기값 설정.
	if (Bullet)
	{
		float BulletAttackPoint = 0.f;

		ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
		if (PS)
		{
			BulletAttackPoint = PS->AttackPoint;
		}

		Bullet->SetDamageInfo(GetController(), BulletAttackPoint, 0, TeamName);
	}



	// 만들어 놓은 몽타주 애셋으로 애니메이션 몽타주 실행. 리로드와 같은 슬롯에 등록되어 있다. (둘다 상체 애니메이션이고 사격과 재장전은 동시에 할 수 없으므로)
	if (FireMontage && FireMontage_Female && FireMontage_Male)
	{
		switch (PlayerMeshType)
		{
		case EMeshType::None:
			CurrentFireMontage = FireMontage;
			break;
		case EMeshType::Female:
			CurrentFireMontage = FireMontage_Female;
			break;
		case EMeshType::Male:
			CurrentFireMontage = FireMontage_Male;
			break;
		default:
			break;
		}

		if (bIsIronsight)
		{
			PlayAnimMontage(CurrentFireMontage, 1.0f, TEXT("Fire_Rifle_Ironsights"));
		}
		else
		{
			PlayAnimMontage(CurrentFireMontage, 1.0f, TEXT("Fire_Rifle_Hip"));
		}
	}

}

// TakeDamage
float ABattleCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (CurrentHP <= 0) return 0.f;

	if (DamageCauser->ActorHasTag(TeamName)) {
		return 0.f;
	}
	
	float TempHP = CurrentHP;
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{

		// 피격 시 HitAnimation
		NetMulticast_StartHitMontage(FMath::RandRange(1, 4));

		//네트워크상에서 CurrentHP동기화를 한번한 하기 위한 장치
		

		FPointDamageEvent* PointDamageEvent = (FPointDamageEvent*)(&DamageEvent);
		if (PointDamageEvent->HitInfo.BoneName.Compare(TEXT("head")) == 0)
		{
			//총, 총알 타입에 따라 헤드샷을 맞았을 때 다른 데미지 들어간다
			TempHP = 0.f;
		}
		else
		{
			TempHP -= DamageAmount;
			UE_LOG(LogTemp, Warning, TEXT("PointDamage:: %f"), DamageAmount);
		}
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		NetMulticast_StartHitMontage(FMath::RandRange(1, 4));
		TempHP -= DamageAmount;
		UE_LOG(LogTemp, Warning, TEXT("RadialDamage:: %f"), DamageAmount);

	}
	else if (DamageEvent.IsOfType(FDamageEvent::ClassID))
	{
		NetMulticast_StartHitMontage(FMath::RandRange(1, 4));
		TempHP -= DamageAmount;
		UE_LOG(LogTemp, Warning, TEXT("NormalDamage:: %f"), DamageAmount);

	}

	TempHP = FMath::Clamp(TempHP, 0.f, 100.f);
	if (CurrentHP != TempHP)
	{
		CurrentHP = TempHP;
		//서버도 실행되게
		OnRep_CurrentHP();
	}
	

	//죽었을때
	if (CurrentHP <= 0)
	{
		NetMulticast_StartDeath(FMath::RandRange(1, 3));
		//CurrentState = EBattleCharacterState::Dead;
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
	UE_LOG(LogTemp,Warning,TEXT("update HP"));
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

FRotator ABattleCharacter::GetAimOffset()
{
	// 카메라 로테이션 벡터를 캐릭터의 트랜스폼 만큼 반대로 이동시킨 로테이션을 구한다. 상대 로테이션.
	return ActorToWorld().InverseTransformVectorNoScale(GetBaseAimRotation().Vector()).Rotation();
}

void ABattleCharacter::NetMulticast_StartDeath_Implementation(int Index)
{
	switch (PlayerMeshType)
	{
	case EMeshType::None:
		CurrentDeathMontage = DeathMontage;
		break;
	case EMeshType::Female:
		CurrentDeathMontage = DeathMontage_Female;
		break;
	case EMeshType::Male:
		CurrentDeathMontage = DeathMontage_Male;
		break;
	default:
		break;
	}

	if (CurrentDeathMontage) {

		FString DeathSectionName = FString::Printf(TEXT("Death%d"), Index);
		PlayAnimMontage(CurrentDeathMontage, 1.f, FName(DeathSectionName));
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
	switch (PlayerMeshType)
	{
	case EMeshType::None:
		CurrentHitActionMontage = HitActionMontage;
		break;
	case EMeshType::Female:
		CurrentHitActionMontage = HitActionMontage_Female;
		break;
	case EMeshType::Male:
		CurrentHitActionMontage = HitActionMontage_Male;
		break;
	default:
		break;
	}
	if (HitActionMontage) {
		FString HitSectionName = FString::Printf(TEXT("Hit%d"), Number);
		PlayAnimMontage(CurrentHitActionMontage, 1.f, FName(HitSectionName));
	}
}

void ABattleCharacter::Server_CallReSpawnToGM_Implementation()
{
	CurrentState = EBattleCharacterState::Dead;
	ABattleGM* GM = Cast<ABattleGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		GM->CallReSpawn(this);
		//Destroy();
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
		WalkSpeed = PS->PlayerSpeed;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void ABattleCharacter::DeathSetting()
{
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	//GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));
	
	// 사망시 입력 막기
	//GetController()->UnPossess();
	DisableInput(GetController<APlayerController>());	
	//GetController<APlayerController>()->SetInputMode(FInputModeUIOnly());

	bIsFire = false;
	bIsIronsight = false;
}

void ABattleCharacter::Server_SetBooty_Implementation(int Money, float Exp)
{
	ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
	if (PS)
	{
		PS->PlayerMoney += Money;
		PS->MyPlayerData.PlayerMoneyData = PS->PlayerMoney;
		ABattlePC* PC = Cast<ABattlePC>(GetController());
		if (PC)
		{
			PC->UpdateGSTabArrayData();
		}

		PS->OnRep_Money();

		PS->PlayerExp += Exp;
		PS->NewExp += Exp;
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

void ABattleCharacter::NetMulticast_InitHPBar_Implementation(ETeamColor color)
{
	FLinearColor ColorRed = FLinearColor(1.f, 0, 0, 1.f);
	FLinearColor ColorBlue = FLinearColor(0, 0, 1.f, 1.f);

	//허드색 변경
	UHPBarWidgetBase* HPWidget = Cast<UHPBarWidgetBase>(Widget->GetUserWidgetObject());
	if (HPWidget)
	{
		UE_LOG(LogClass, Warning, TEXT("Player HUD is Vaild"));
		if (color == ETeamColor::Red)
		{
			HPWidget->SetColorAndOpacity(ColorRed);
		}
		else if (color == ETeamColor::Blue)
		{
			HPWidget->SetColorAndOpacity(ColorBlue);
		}
	}

	//위젯색 변경
	ABattlePC* PC = Cast<ABattlePC>(GetController());
	if (PC && PC->IsLocalController())
	{
		if (color == ETeamColor::Red)
		{
			PC->BattleWidgetObject->SetHPBarColor(ColorRed);
		}
		else if (color == ETeamColor::Blue)
		{
			PC->BattleWidgetObject->SetHPBarColor(ColorBlue);
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

void ABattleCharacter::Server_ItemAttack_Implementation()
{
	ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
	if (PS)
	{
		PS->AttackPoint += 10.f;
		PS->MyPlayerData.PlayerAttackData = PS->AttackPoint;
		ABattlePC* PC = Cast<ABattlePC>(GetController());
		if (PC)
		{
			PC->UpdateGSTabArrayData();
		}
	}
}

void ABattleCharacter::Server_ItemSpeed_Implementation()
{
	ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
	if (PS)
	{
		PS->PlayerSpeed += 100.f;
		PS->MyPlayerData.PlayerSpeedData = PS->PlayerSpeed;
		ABattlePC* PC = Cast<ABattlePC>(GetController());
		if (PC)
		{
			PC->UpdateGSTabArrayData();
		}
	}
	WalkSpeed += 100.f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	Server_SetMaxWalkSpeed(WalkSpeed);
	RunSpeed += 100.f;
}

void ABattleCharacter::Server_ItemHP_Implementation()
{
	CurrentHP = MaxHP;
	OnRep_CurrentHP();
}

void ABattleCharacter::NetMulticast_SetMeshSettings_Implementation(const EMeshType& MyMeshType)
{
	PlayerMeshType = MyMeshType;
	
	switch (PlayerMeshType)
	{
	case EMeshType::None:
		break;
	case EMeshType::Female:
		GetMesh()->SetSkeletalMesh(FemaleMesh);
		GetMesh()->SetAnimClass(FemaleAnim);
		break;
	case EMeshType::Male:
		GetMesh()->SetSkeletalMesh(MaleMesh);
		GetMesh()->SetAnimClass(MaleAnim);
		break;
	default:
		break;
	}
}

