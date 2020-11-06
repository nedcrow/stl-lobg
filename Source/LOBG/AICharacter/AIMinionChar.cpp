// Fill out your copyright notice in the Description page of Project Settings.


#include "AIMinionChar.h"

#include "MinionAIC.h"
#include "Fairy/FairyPawn.h"
#include "../Weapon/WeaponComponent.h"
#include "../Weapon/EmissiveBullet.h"
#include "../Battle/BattleGM.h"
#include "../Battle/BattlePC.h"
#include "../Battle/BattleCharacter.h"
#include "../UI/HUDBarSceneComponent.h"
#include "../UI/HPBarWidgetBase.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/PawnSensingComponent.h"
//#include "Sound/SoundAttenuation.h"

// Sets default values
AAIMinionChar::AAIMinionChar()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Collision
	GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);		// Player ä��. ���Ǳ� ���� ������ Ȯ�� �ʿ�.
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//if (RoboMeshes)
	//{
	//	GetMesh()->SetSkeletalMesh(RoboMeshes);		// �޽� ����.
	//}

	TeamLampEye = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeamLampEye"));
	TeamLampEye->SetupAttachment(GetMesh(), TEXT("head"));
	TeamLampEye->SetRelativeLocationAndRotation(FVector(11.3f, 3.36f, 0.f), FRotator(0.f, 90.f, 45.f));
	TeamLampEye->SetCollisionProfileName(TEXT("NoCollision"), false);
	TeamLampEye->SetGenerateOverlapEvents(false);
	TeamLampEye->PrimaryComponentTick.bCanEverTick = false;
	TeamLampEye->SetRelativeScale3D(FVector(0.5f, 1.f, 1.f));

	TeamLampBack = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeamLampBack"));
	TeamLampBack->SetupAttachment(GetMesh(), TEXT("spine_03"));
	TeamLampBack->SetRelativeLocationAndRotation(FVector(22.4f,16.1f, 0.f), FRotator(90.f, 0.f, 37.f));
	TeamLampBack->SetCollisionProfileName(TEXT("NoCollision"), false);
	TeamLampBack->SetGenerateOverlapEvents(false);
	TeamLampBack->PrimaryComponentTick.bCanEverTick = false;
	TeamLampBack->SetRelativeScale3D(FVector(0.6f, 1.63f, 1.f));

	// Weapon
	Weapon = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));

	// Movement
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// AI
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = PawnSensingSightRadius;
	PawnSensing->bOnlySensePlayers = false;
	PawnSensing->bHearNoises = false;
	PawnSensing->SetPeripheralVisionAngle(45.f);
	PawnSensing->SetActive(false);

	// UI
	HPBarHUD = CreateDefaultSubobject<UHUDBarSceneComponent>(TEXT("HPBarHUD"));
	HPBarHUD->SetupAttachment(RootComponent);
	HPBarHUD->SetRelativeLocation(FVector(0, 0, 110.f));

	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->SetupAttachment(HPBarHUD);
	Widget->SetRelativeRotation(FRotator(0, 180.f, 0));

	// ETC
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	Tags.Add(TEXT("Minion"));

	// �ʱ�ȭ
	CurrentHP = MaxHP;
	CurrentState = EMinioonState::Normal;

	//AttenuationSettings = CreateDefaultSubobject<USoundAttenuation>("AttenuationSettings");
	//AttenuationSettings->Attenuation.bAttenuate = true;
}

// Called when the game starts or when spawned
void AAIMinionChar::BeginPlay()
{
	Super::BeginPlay();

	// �ʱ�ȭ
	InitHPBar();
	UpdateHPBar();

	if (GetWorld()->IsServer())
	{
		SetState(CurrentState);

		if (PawnSensing)
		{
			PawnSensing->OnSeePawn.AddDynamic(this, &AAIMinionChar::ProcessSeenPawn);
		}
	}

}

// PawnSensing
void AAIMinionChar::ProcessSeenPawn(APawn * Pawn)
{
	if (CurrentState != EMinioonState::Normal || Pawn->ActorHasTag(TeamName))
	{
		return;
	}


	//if (Pawn->ActorHasTag("Minion"))
	//{

	//}
	//else if (Pawn->ActorHasTag("Player"))
	//{
	//	Pawn->GetPlayerState();
	//	//ABattleCharacter * PlayerPawn = Cast<ABattleCharacter>(Pawn);
	//	//if (PlayerPawn->TeamColor == TeamColor)
	//	//{

	//	//}
	//}
	//else if (Pawn->ActorHasTag("Tower"))
	//{
	//	AFairyPawn * FAiryPawn = Cast<AFairyPawn>(Pawn);
	//	//if (FAiryPawn->TeamColor == TeamColor)
	//	//{

	//	//}
	//}

	//AMinionAIC* MinionAIC = GetController<AMinionAIC>();
	//if (MinionAIC)
	//{
	//}



	// �ڱ��� �� ���� BB�� �Է��Ѵ�.
	AMinionAIC* MinionAIC = Cast<AMinionAIC>(GetController());
	if (MinionAIC && !MinionAIC->IsActorBeingDestroyed())
	{
		MinionAIC->SetValueTargetPawn(Pawn);

		SetState(EMinioonState::Chase);
	}

}

// Called every frame
void AAIMinionChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAIMinionChar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME_CONDITION(ATPSCharacter, bIsFire, COND_SimulatedOnly);
	DOREPLIFETIME(AAIMinionChar, MaxHP);
	DOREPLIFETIME(AAIMinionChar, CurrentHP);
	DOREPLIFETIME(AAIMinionChar, CurrentState);
	DOREPLIFETIME(AAIMinionChar, bIsFire);
	DOREPLIFETIME(AAIMinionChar, TeamName);

}

// Called to bind functionality to input
void AAIMinionChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAIMinionChar::OnRep_CurrentHP()
{
	UpdateHPBar();
}

void AAIMinionChar::OnRep_TeamName()
{
	Tags.AddUnique(TeamName);

	if (RoboMeshes)
	{
		GetMesh()->SetSkeletalMesh(RoboMeshes);		// �޽� ����.
	}

	// ���� ��Ƽ���� ����.
	if (TeamName == TEXT("Red"))
	{
		if (RoboMaterials.IsValidIndex(0))
		{
			GetMesh()->SetMaterial(0, RoboMaterials[0]);
		}

		if (TeamLampMaterials.IsValidIndex(0))
		{
			TeamLampEye->SetMaterial(0, TeamLampMaterials[0]);
			TeamLampBack->SetMaterial(0, TeamLampMaterials[0]);
		}
	}
	else if (TeamName == TEXT("Blue"))
	{
		if (RoboMaterials.IsValidIndex(1))
		{
			GetMesh()->SetMaterial(0, RoboMaterials[1]);
		}

		if (TeamLampMaterials.IsValidIndex(1))
		{
			TeamLampEye->SetMaterial(0, TeamLampMaterials[1]);
			TeamLampBack->SetMaterial(0, TeamLampMaterials[1]);
		}
	}
}

void AAIMinionChar::SetTeamName(FName MyTeamName)
{
	TeamName = MyTeamName;
	OnRep_TeamName();

	InitHPBar();
}

void AAIMinionChar::OnRep_CurrentState()
{
	// ���� ����ȭ�� ���ؼ� ����.
}

void AAIMinionChar::SetState(EMinioonState NewState)
{
	CurrentState = NewState;

	// BB Ű�� �Է�.
	AMinionAIC* MinionAIC = GetController<AMinionAIC>();
	if (MinionAIC)
	{
		MinionAIC->SetValueState(NewState);
	}

	// ��ֻ��¿��� ������ �۵�.
	if (NewState == EMinioonState::Normal)
	{
		PawnSensing->SetActive(true);
	}
	else
	{
		PawnSensing->SetActive(false);

	}
}

// Fire
void AAIMinionChar::OnFire(FVector TargetLocation)
{
	// ���� ���� ���ϱ�
	// �Ѿ� ����
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = (Weapon != nullptr) ? Weapon->GetSocketLocation(TEXT("Muzzle")) : GetActorLocation() + GetActorForwardVector() * 100.f;
			//const FRotator SpawnRotation = GetControlRotation();
			//const FRotator SpawnRotation = (TargetLocation - SpawnLocation).Rotation();
			FRotator SpawnRotation = (Weapon != nullptr) ? FRotator(AimPitch, GetActorRotation().Yaw, 0.f) : FRotator(AimPitch, GetActorRotation().Yaw, 0.f);



			// �ѱ��� Ÿ�� ���̰� ����ִ��� �˻��Ѵ�. �½�ũ�� ���� �Լ��� ���� �ϰ�����, �̰��� ���������� �ľ��ϱ� ������ �ϴ� ���⿡ �ִ´�.
			TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel2));		// Player ä��
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel3));		// SeeTrough ä��

			TArray<AActor*> ActorsToIgnore;

			FHitResult OutHit;

			bool bResult = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), SpawnLocation, SpawnLocation + SpawnRotation.Vector() * 1600.f,
				ObjectTypes, true, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true, FLinearColor::Red, FLinearColor::Green, 5.0f);

			// ��Ʈ ����� ���� ���̸� ���
			if (bResult)
			{
				//FName EnemyTeam;
				//if (TeamName == TEXT("Red"))
				//{
				//	EnemyTeam = TEXT("Blue");
				//}
				//else if (TeamName == TEXT("Blue"))
				//{
				//	EnemyTeam = TEXT("Red");
				//}

				// �Ʊ��̳� ���� �ƴ� �͵��� ��Ʈ�Ǹ� ��� ����.
				if (!Cast<APawn>(OutHit.Actor) || OutHit.Actor->ActorHasTag(TeamName))
				{
					// ���߽�ų �� ���� ��� �̵�.
					SetState(EMinioonState::Normal);

					return;
				}

				// ���� ��ȭ�����ڶ� �������� �� ����. BTTASK���� ���� üũ�ؼ� ����� ��� �� ��.
				if (OutHit.GetActor()->ActorHasTag("Minion")) {
					AAIMinionChar* Actor = Cast<AAIMinionChar>(OutHit.GetActor());
					if (Actor && Actor->CurrentState == EMinioonState::Dead) {
						SetState(EMinioonState::Normal);
						return;
					}
				}
				else if (OutHit.GetActor()->ActorHasTag("Player")) {
					ABattleCharacter* Actor = Cast<ABattleCharacter>(OutHit.GetActor());
					if (Actor && Actor->CurrentState == EBattleCharacterState::Dead) {
						SetState(EMinioonState::Normal);
						return;
					}
				}
				else if (OutHit.GetActor()->ActorHasTag("Tower")) {
					AFairyPawn* Actor = Cast<AFairyPawn>(OutHit.GetActor());
					if (Actor && Actor->CurrentState == EFairyState::Death) {
						SetState(EMinioonState::Normal);
						return;
					}
				}
			}

			// �Ѿ� Ȯ��.
			SpawnRotation.Yaw += FMath::FRandRange(-1.f, 1.f);
			SpawnRotation.Pitch += FMath::FRandRange(-1.f, 1.f);

			// ��� Ŭ���̾�Ʈ���� �Ѿ� ����.
			NetMulticast_ProcessFire(SpawnLocation, SpawnRotation);
		}
	}
	else
	{
		SetState(EMinioonState::Normal);

		return;
	}

	
	// ��� �ļ� ����Ʈ
	//NetMulticast_ProcessFire();
}

void AAIMinionChar::NetMulticast_ProcessFire_Implementation(FVector SpawnLocation, FRotator SpawnRotation)
{
	//Set Spawn Collision Handling Override
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// spawn the projectile at the muzzle
	AEmissiveBullet* SpawnedBullet = GetWorld()->SpawnActor<AEmissiveBullet>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	// �Ѿ� �ʱ�ȭ
	if (SpawnedBullet)
	{
		SpawnedBullet->SetDamageInfo(GetController(), AttackDamage, TeamName);
	}




	// �Ҹ� ����
	if (FireSound) 
	{

		UGameplayStatics::SpawnSoundAtLocation(GetWorld(),
			FireSound,
			GetActorLocation()
		);
	}

	// ��� ��Ÿ�� ���
	if (FireMontage)
	{
		PlayAnimMontage(FireMontage, 1.f, TEXT("Fire_Rifle_Hip"));
	}




}

// TakeDamage
float AAIMinionChar::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (CurrentHP <= 0) 
	{
		return DamageAmount;
	}
	
	if (!GetWorld()->IsServer())
	{
		if (!DamageCauser->ActorHasTag(TeamName))
		{
			// �ǰ� ȿ�� Ŭ���̾�Ʈ��. ������ �Է��� ������ ����� ���� �ʴ´�.
			BP_HitEffect();
		}

		return 0.f;
	}

	// ������ ���.
	float TempHP = CurrentHP;
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{

		//Destroy();

		// �ǰ� �� HitAnimation
		// NetMulticast_StartHitMontage(FMath::RandRange(1, 4))

		//��Ʈ��ũ�󿡼� CurrentHP����ȭ�� �ѹ��� �ϱ� ���� ��ġ
		

		FPointDamageEvent* PointDamageEvent = (FPointDamageEvent*)(&DamageEvent);
		if (PointDamageEvent->HitInfo.BoneName.Compare(TEXT("head")) == 0)
		{
			//��, �Ѿ� Ÿ�Կ� ���� ��弦�� �¾��� �� �ٸ� ������ ����

			TempHP = 0.f;
		}
		else
		{
			TempHP -= DamageAmount;
		}


	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		TempHP -= DamageAmount;
		UE_LOG(LogTemp, Warning, TEXT("Minion RadialDamage:: %f"), DamageAmount);

	}
	else if (DamageEvent.IsOfType(FDamageEvent::ClassID))
	{
		TempHP -= DamageAmount;


	}

	// HP �Է�.
	TempHP = FMath::Clamp(TempHP, 0.f, MaxHP);
	if (CurrentHP != TempHP)
	{
		CurrentHP = TempHP;
		OnRep_CurrentHP();		// ���� ���� ����
	}

	//�׾�����
	if (CurrentHP <= 0)
	{
		NetMulticast_StartDeath(FMath::RandRange(1, 3));
		SetState(EMinioonState::Dead);
		SetLifeSpan(5.f);

		//�����÷��̾�� ����
		ABattlePC* PC = Cast<ABattlePC>(EventInstigator);
		if (PC)
		{
			ABattleCharacter* Pawn = Cast<ABattleCharacter>(PC->GetPawn());
			if (Pawn)
			{
				Pawn->Server_SetBooty(MinionMoney, MinionExp);
			}
		}
	}
	else if(EventInstigator)
	{
		// �ǰݽ� ���� ���.
		// �Ѿ��� ���� ���� �̹� ������� ���ɼ��� �����Ƿ� ���� �����ϴ��� üũ�Ѵ�.
		APawn* EnemyPawn = EventInstigator->GetPawn();
		if (EnemyPawn)
		{
			// �ǰݽ� ������ ���� �Լ��� ��� ������Ʈ�� ���� BB�� ���� ����ϰ� �����ϵ��� �ٲ۴�.
			ProcessSeenPawn(EnemyPawn);

			// �÷��̾�� �ǰݵ� ��� �����߿��� Ÿ���� �÷��̾�� �ٲ۴�.
			if (EnemyPawn->ActorHasTag(TEXT("Player")))
			{
				// �ڱ��� �� ���� BB�� �Է��Ѵ�.
				AMinionAIC* MinionAIC = Cast<AMinionAIC>(GetController());
				if (MinionAIC && !MinionAIC->IsActorBeingDestroyed())
				{
					MinionAIC->SetValueTargetPawn(EnemyPawn);

					SetState(EMinioonState::Chase);
				}
			}
		}
	}

	// �ǰ� ȿ��. ������.
	BP_HitEffect();

	return DamageAmount;
}

void AAIMinionChar::NetMulticast_StartDeath_Implementation(int Index)
{
	if (DeathMontage && !Montage_IsPlaying(DeathMontage))
	{
		FString DeathSectionName = FString::Printf(TEXT("Death%d"), Index);
		PlayAnimMontage(DeathMontage, 1.f, FName(DeathSectionName));

	}

	DeathSetting();
}

void AAIMinionChar::DeathSetting()
{
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	//GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));

	// ��Ƽ���� ȿ��
}

bool AAIMinionChar::Montage_IsPlaying(UAnimMontage * AnimMontage)
{
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (AnimMontage && AnimInstance)
	{
		return AnimInstance->Montage_IsPlaying(AnimMontage);
	}


	return false;
}

// UI
void AAIMinionChar::InitHPBar()
{
	// ���� �� ����
	UHPBarWidgetBase* HPWidget = Cast<UHPBarWidgetBase>(Widget->GetUserWidgetObject());
	if (HPWidget)
	{
		if (TeamName == TEXT("Red"))
		{
			HPWidget->SetColorAndOpacity(FLinearColor::Red);
		}
		else if (TeamName == TEXT("Blue"))
		{
			HPWidget->SetColorAndOpacity(FLinearColor::Blue);
		}
	}

}


void AAIMinionChar::UpdateHPBar()
{
	UHPBarWidgetBase* HPWidget = Cast<UHPBarWidgetBase>(Widget->GetUserWidgetObject());
	if (HPWidget && MaxHP > 0)
	{
		HPWidget->SetHPBar(CurrentHP / MaxHP);
	}
}
