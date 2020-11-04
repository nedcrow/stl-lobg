// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "BattlePC.h"
#include "BattleGM.h"
#include "BattlePS.h"
#include "BattleWidgetBase.h"
#include "../Lobby/UI/ChattingWidgetBase.h"
#include "CharacterAnimInstance.h"
#include "../Weapon/BulletBase.h"
#include "../ReSpawn/ReSpawn.h"
#include "Kismet/GameplayStatics.h"
#include "../LOBGGameInstance.h"
#include "../Weapon/WeaponComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/EditableTextBox.h"
#include "../UI/HPBarWidgetBase.h"
#include "../UI/HUDBarSceneComponent.h"
#include "Components/SphereComponent.h"
#include "../Store/StoreWidgetBase.h"
#include "../Store/StoreItemBoxWidgetBase.h"
#include "Engine/StreamableManager.h"

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

	ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
	if (PS)
	{
		MaxHP = PS->MaxHP;

		CurrentHP = MaxHP;
	}
	CurrentState = EBattleCharacterState::Normal;

	//UI초기화 및 유무 확인
	OnRep_CurrentHP();
	if (PS)
	{
		PS->OnRep_Exp();
		PS->OnRep_Money();
		//WalkSpeed = PS->PlayerSpeed;
		NetMulticast_InitHPBar_Implementation(PS->TeamColor);
	}

	//if (IsLocallyControlled())
	//{
	//	if (PS)
	//	{
	//		CurrentGunName = PS->CurrentGun;
	//		UE_LOG(LogClass, Warning, TEXT("CurrentGunName is %s"), *CurrentGunName);
	//	}
	//
	//	ABattlePC* PC = Cast<ABattlePC>(GetController());
	//
	//	if (CurrentGunName == FString(""))
	//	{
	//		if (PC)
	//		{
	//			PC->StoreWidgetObject->ItemBox->CheckSleepSlot();
	//		}
	//	}
	//	else
	//	{
	//		if (PC)
	//		{
	//			PC->StoreWidgetObject->ItemBox->WakeUpSlot();
	//			for (int i = 0; i < PS->GunDataArray.Num(); ++i)
	//			{
	//				if (PS->GunDataArray[i].GunName == CurrentGunName)
	//				{
	//					PC->StoreWidgetObject->ItemBox->SetUpgradeGunUpdate(CurrentGunName, PS->GunDataArray[i].DataTableIndex);
	//				}
	//			}
	//		}
	//	}
	//}
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

	PlayerInputComponent->BindAction(TEXT("GoHome"), IE_Pressed, this, &ABattleCharacter::GoHome);

	PlayerInputComponent->BindAction(TEXT("EatPotion"), IE_Pressed, this, &ABattleCharacter::EatPotion);

	PlayerInputComponent->BindAction(TEXT("CheatKey"), IE_Pressed, this, &ABattleCharacter::PressCheatKey);
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
	DOREPLIFETIME(ABattleCharacter, BulletSpeed);
	DOREPLIFETIME(ABattleCharacter, bIsLeanLeft);
	DOREPLIFETIME(ABattleCharacter, bIsLeanRight);
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
	
	ABattlePC* PC = GetController<ABattlePC>();
	if (PC && PC->BattleWidgetObject->ChattingWidget->ChatInput->Visibility == ESlateVisibility::Visible) {
		PC->BattleWidgetObject->ChattingWidget->ExitInputBox(PC);
	}
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
	ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
	if (PS)
	{

		float FireAngle = PS->FireBulletAngle;
		if (bIsIronsight && FireAngle != 0)
		{
			//조준 하면 확산각 하락.
			FireAngle = PS->FireBulletAngle / PS->FireBulletAngle;
		}

		if (GetCharacterMovement()->IsFalling())
		{
			// 점프한 상태면 확산각 증가. 
			FireAngle *= 2.f;
		}
		BulletRoation.Yaw += FMath::FRandRange(-FireAngle, FireAngle);
		BulletRoation.Pitch += FMath::FRandRange(-FireAngle, FireAngle);
	}


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
	ActorSpawnParams.Owner = this;

	ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
	if (PS)
	{
		BulletSpeed = PS->BulletSpeed;
	}
	// 총알 스폰
	//ABulletBase* Bullet = GetWorld()->SpawnActor<ABulletBase>(BulletClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	
	FTransform BulletTransform = FTransform(SpawnRotation, SpawnLocation);

	ABulletBase* Bullet = Cast<ABulletBase>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), BulletClass, BulletTransform));
	
	// 총알 초기값 설정.
	if (Bullet)
	{
		//총알 속도 설정
		Bullet->AddSpeed(BulletSpeed);

		float BulletAttackPoint = 0.f;
	
		if (PS)
		{
			BulletAttackPoint = PS->AttackPoint;
		}
	
		Bullet->SetDamageInfo(GetController(), BulletAttackPoint, 0, TeamName);
	
		// 총알에 캐릭터의 관성을 추가한다. 적용 취소. 피직스를 키면 총알 물리가 이상해짐;;
		//Bullet->Sphere->AddImpulse(GetVelocity());

		//스폰 후 초기설정 후 완료작업 : DererredActor를 실행한 후 반드시 필요하다
		UGameplayStatics::FinishSpawningActor(Bullet, BulletTransform);
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

	if (CurrentHP <= 0)
	{
		return 0.f;
	}

	if (!GetWorld()->IsServer() || DamageCauser->ActorHasTag(TeamName))
	{
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

	}

	TempHP = FMath::Clamp(TempHP, 0.f, MaxHP);
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
}

void ABattleCharacter::Server_SetIronsight_Implementation(bool State)
{
	bIsIronsight = State;
}

void ABattleCharacter::StartIronsight()
{
	bIsIronsight = true;
	Server_SetIronsight(true);
	ABattlePC* PC = GetController<ABattlePC>();
	if (PC && PC->BattleWidgetObject->ChattingWidget->ChatInput->Visibility == ESlateVisibility::Visible) {
		PC->BattleWidgetObject->ChattingWidget->ExitInputBox(PC);
	}
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

	if (CurrentDeathMontage)
	{

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
	if (HitActionMontage)
	{
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

		//GM의 함수로 가서 Pawn의 컨트롤러도 가져오고 CurrentState도 검사해야하는데 Destroy()되면 할 수 없기 때문
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
		ABattlePC* PC = Cast<ABattlePC>(GetController());
		if (PC)
		{
			PC->StoreWidgetObject->ItemBox->CheckAllSlotActive();
		}
	}
	Server_SetBooty(Money, Exp);
}

void ABattleCharacter::NetMulticast_AddTag_Implementation(const FName & PlayerTag)
{
	Tags.Add(PlayerTag);
	TeamName = PlayerTag;
}

void ABattleCharacter::NetMulticast_InitHPBar_Implementation(ETeamColor color)
{
	ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
	if (PS)
	{
		MaxHP = PS->MaxHP;
		CurrentHP = MaxHP;
	}

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
}

void ABattleCharacter::ItemSpeed()
{
	WalkSpeed += 100.f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	Server_SetMaxWalkSpeed(WalkSpeed);
	Server_ItemSpeed();
}

void ABattleCharacter::Server_FullHP_Implementation()
{
	ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
	if (PS)
	{
		MaxHP = PS->MaxHP;
		UE_LOG(LogClass, Warning, TEXT("InReSpawn MaxHP is %f"), MaxHP);
		CurrentHP = PS->MaxHP;
		UE_LOG(LogClass, Warning, TEXT("InReSpawn CurrentHP is %f"), CurrentHP);
		OnRep_CurrentHP();
	}
	//CurrentHP = MaxHP;
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

void ABattleCharacter::Server_AngleDown_Implementation()
{
	ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
	if (PS)
	{
		PS->FireBulletAngle = 0.f;
	}
}

void ABattleCharacter::Server_SetPSFOV_Implementation()
{
	ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
	if (PS)
	{
		PS->PlayerFOV = 30.f;
	}
}

void ABattleCharacter::SetPSFOV()
{
	ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
	if (PS)
	{
		PS->PlayerFOV = 30.f;
	}
}

void ABattleCharacter::GoHome()
{
	Server_GoHome();
}

void ABattleCharacter::Server_GoHome_Implementation()
{
	FVector HomeLocation;

	TArray<AActor*> RedReSpawnArray;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AReSpawn::StaticClass(), TEXT("Red"), RedReSpawnArray);

	TArray<AActor*> BlueReSpawnArray;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AReSpawn::StaticClass(), TEXT("Blue"), BlueReSpawnArray);

	ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
	if (PS)
	{
		switch (PS->TeamColor)
		{
		case ETeamColor::Red:
			HomeLocation = RedReSpawnArray[0]->GetActorLocation();
			break;
		case ETeamColor::Blue:
			HomeLocation = BlueReSpawnArray[0]->GetActorLocation();
			break;
		default:
			break;
		}
	}

	SetActorRelativeLocation(HomeLocation);
}

void ABattleCharacter::SetPotionSlot()
{
	ABattlePC* PC = Cast<ABattlePC>(GetController());
	if (PC)
	{
		for (int i = 0; i < 5; ++i)
		{
			if (PC->StoreWidgetObject->ItemBox->GetItemData(i).ItemName == FString("Potion"))
			{
				FStreamableManager loader;
				PC->BattleWidgetObject->SetPotionSlot(loader.LoadSynchronous<UMaterialInstance>(PC->StoreWidgetObject->ItemBox->GetItemData(i).ItemImage));
			}
		}
	}
}

void ABattleCharacter::EatPotion()
{
	Server_FullHP();

	ABattlePC* PC = Cast<ABattlePC>(GetController());
	if (PC)
	{
		PC->BattleWidgetObject->EmptyPotionSlot();
	}
}

void ABattleCharacter::Server_ChangeGunMesh_Implementation(USkeletalMesh* GunMesh)
{
	NetMulticast_ChangeGunMesh(GunMesh);
}

void ABattleCharacter::NetMulticast_ChangeGunMesh_Implementation(USkeletalMesh* GunMesh)
{
	Weapon->SetSkeletalMesh(GunMesh);
}

void ABattleCharacter::ChangeGunMesh(const FString& GunItemName)
{
	//CurrentGunName = GunItemName;

	ABattlePC* PC = Cast<ABattlePC>(GetController());
	if (PC)
	{
		for (int i = 0; i < 9; ++i)
		{
			if (PC->StoreWidgetObject->ItemBox->GetItemData(i).ItemName == GunItemName)
			{
				FStreamableManager loader;
				Weapon->SetSkeletalMesh(loader.LoadSynchronous<USkeletalMesh>(PC->StoreWidgetObject->ItemBox->GetItemData(i).GunMesh));
				Server_ChangeGunMesh(loader.LoadSynchronous<USkeletalMesh>(PC->StoreWidgetObject->ItemBox->GetItemData(i).GunMesh));

				ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
				if (PS)
				{
					PS->CurrentGun = GunItemName;
					CurrentGunName = GunItemName;

					if (PS->GunDataArray.Num() == 0)
					{
						FGunUpgradeData MyGunData;
						MyGunData.GunName = GunItemName;
						MyGunData.UpgradeCount = 1;
						MyGunData.DataTableIndex = i;
						PS->GunDataArray.Emplace(MyGunData);
						SwitchAttackpoint(0);
					}
					else
					{
						for (int arrayindex = 0; arrayindex < PS->GunDataArray.Num(); ++arrayindex)
						{
							if (PS->GunDataArray[arrayindex].GunName == GunItemName)
							{
								PS->GunDataArray[arrayindex].UpgradeCount = 1;
								SwitchAttackpoint(arrayindex);
								break;
							}
							else if (arrayindex == PS->GunDataArray.Num() - 1)
							{
								FGunUpgradeData MyGunData;
								MyGunData.GunName = GunItemName;
								MyGunData.UpgradeCount = 1;
								MyGunData.DataTableIndex = i;
								PS->GunDataArray.Emplace(MyGunData);
								SwitchAttackpoint(PS->GunDataArray.Num() - 1);
							}
						}
					}
				}

				PC->StoreWidgetObject->ItemBox->WakeUpSlot();
				PC->StoreWidgetObject->ItemBox->SetUpgradeGunUpdate(CurrentGunName, i);
				PC->BattleWidgetObject->SetCurrentGunText(CurrentGunName);
				PC->BattleWidgetObject->SetCurrentGunUpgradeText(1);
				break;
			}
		}
	}
}

void ABattleCharacter::Server_MaxHPUp_Implementation()
{
	ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
	if (PS)
	{
		PS->MaxHP += 50.f;
	}

	MaxHP += 50.f;
	CurrentHP = MaxHP;
	OnRep_CurrentHP();
}

void ABattleCharacter::Server_BulletSpeedUp_Implementation()
{
	ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
	if (PS)
	{
		PS->BulletSpeed += 10000.f;
	}
	//BulletSpeed += 200.f;
}

void ABattleCharacter::GunUpgrade()
{
	ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
	if (PS)
	{
		for (int i = 0; i < PS->GunDataArray.Num(); ++i)
		{
			if (PS->GunDataArray[i].GunName == CurrentGunName)
			{
				PS->GunDataArray[i].UpgradeCount++;

				ABattlePC* PC = Cast<ABattlePC>(GetController());
				if (PC)
				{
					if (PS->GunDataArray[i].UpgradeCount == 3)
					{
						PC->StoreWidgetObject->ItemBox->CheckSleepSlot();
					}
					else
					{
						PC->StoreWidgetObject->ItemBox->SetUpgradeGunUpdate(CurrentGunName, PS->GunDataArray[i].DataTableIndex);
					}

					PC->BattleWidgetObject->SetCurrentGunUpgradeText(PS->GunDataArray[i].UpgradeCount);

					SwitchAttackpoint(i);
				}
			}
		}
	}
}

void ABattleCharacter::Server_AttackUpdate_Implementation(float AttackDamage)
{
	ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
	if (PS)
	{
		PS->AttackPoint = AttackDamage;
		UE_LOG(LogClass, Warning, TEXT("AttackPoint is %f"), PS->AttackPoint);
	}
}

void ABattleCharacter::SwitchAttackpoint(int index)
{
	ABattlePS* PS = Cast<ABattlePS>(GetPlayerState());
	if (PS)
	{
		ABattlePC* PC = Cast<ABattlePC>(GetController());
		if (PC)
		{
			switch (PS->GunDataArray[index].UpgradeCount)
			{
			case 0:
				UE_LOG(LogClass, Warning, TEXT("case is 0"));
				break;
			case 1:
				Server_AttackUpdate(PC->StoreWidgetObject->ItemBox->GetItemData(PS->GunDataArray[index].DataTableIndex).GunAPU1);
				break;
			case 2:
				Server_AttackUpdate(PC->StoreWidgetObject->ItemBox->GetItemData(PS->GunDataArray[index].DataTableIndex).GunAPU2);
				break;
			case 3:
				Server_AttackUpdate(PC->StoreWidgetObject->ItemBox->GetItemData(PS->GunDataArray[index].DataTableIndex).GunAPU3);
				break;
			default:
				break;
			}
		}
	}
}

void ABattleCharacter::PressCheatKey()
{
	SetBooty(1000.f, 0);
}

