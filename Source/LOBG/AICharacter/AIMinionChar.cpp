// Fill out your copyright notice in the Description page of Project Settings.


#include "AIMinionChar.h"

#include "MinionAIC.h"
#include "../Weapon/WeaponComponent.h"
#include "../Weapon/EmissiveBullet.h"
#include "../Battle/BattleCharacter.h"
#include "Fairy/FairyPawn.h"
#include "../UI/HUDBarSceneComponent.h"
#include "../UI/HPBarWidgetBase.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
AAIMinionChar::AAIMinionChar()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Collision
	GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()), FRotator(0.f, -90.f, 0.f));

	// Weapon
	Weapon = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));

	// Movement
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// AI
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));

	// UI
	HPBarHUD = CreateDefaultSubobject<UHUDBarSceneComponent>(TEXT("HPBarHUD"));
	HPBarHUD->SetupAttachment(RootComponent);
	HPBarHUD->SetRelativeLocation(FVector(0, 0, 110));

	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->SetupAttachment(HPBarHUD);
	Widget->SetRelativeRotation(FRotator(0, 180.f, 0));

	// ETC
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	Tags.Add(TEXT("Minion"));
}

// Called when the game starts or when spawned
void AAIMinionChar::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;

	SetState(EMinioonState::Normal);

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AAIMinionChar::ProcessSeenPawn);
	}	

}

// PawnSensing
void AAIMinionChar::ProcessSeenPawn(APawn * Pawn)
{
	if (CurrentState != EMinioonState::Normal || Pawn->ActorHasTag(TeamName))
	{
		return;
	}

	UE_LOG(LogClass, Warning, TEXT("ProcessSeenPawn()"));

	if (Pawn->ActorHasTag("Minion"))
	{

	}
	else if (Pawn->ActorHasTag("Player"))
	{
		Pawn->GetPlayerState();
		//ABattleCharacter * PlayerPawn = Cast<ABattleCharacter>(Pawn);
		//if (PlayerPawn->TeamColor == TeamColor)
		//{

		//}
	}
	else if (Pawn->ActorHasTag("Tower"))
	{
		AFairyPawn * FAiryPawn = Cast<AFairyPawn>(Pawn);
		//if (FAiryPawn->TeamColor == TeamColor)
		//{

		//}
	}

	//AMinionAIC* MinionAIC = GetController<AMinionAIC>();
	//if (MinionAIC)
	//{
	//}


	AMinionAIC* MinionAIC = Cast<AMinionAIC>(GetController());
	if (MinionAIC)
	{
		MinionAIC->SetValueTargetPawn(Pawn);
	}

	SetState(EMinioonState::Chase);
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

void AAIMinionChar::OnRep_CurrentState()
{
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

// Fire
void AAIMinionChar::OnFire(FVector TargetLocation)
{
	// 액터 방향 구하기
	// 총알 스폰
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			//const FRotator SpawnRotation = GetControlRotation();
			const FRotator SpawnRotation = (TargetLocation - GetActorLocation()).Rotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = (Weapon != nullptr) ? Weapon->GetSocketLocation(TEXT("Muzzle")) : GetActorLocation() + SpawnRotation.RotateVector(FVector(100.0f, 0.0f, 10.0f));

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			// spawn the projectile at the muzzle
			World->SpawnActor<AEmissiveBullet>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}
	else
	{
		return;
	}

	
	// 사격 후속 이펙트
	NetMulticast_ProcessFire();
}

void AAIMinionChar::NetMulticast_ProcessFire_Implementation()
{
	// 소리 스폰

	// 사격 몽타주 재생
	if (FireMontage)
	{
		PlayAnimMontage(FireMontage, 1.f, TEXT("Fire_Rifle_Hip"));
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

