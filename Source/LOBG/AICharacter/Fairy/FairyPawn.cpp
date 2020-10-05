// Fill out your copyright notice in the Description page of Project Settings.


#include "FairyPawn.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "../../Battle/BattleCharacter.h"
#include "../../Battle/BattlePC.h"
#include "MeshesRing.h"
#include "MeshesRingComponent.h"
#include "FairyAIController.h"
#include "Engine/StaticMeshActor.h"
#include "UObject/Class.h"
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
}

// Called when the game starts or when spawned
void AFairyPawn::BeginPlay()
{
	Super::BeginPlay();	

	NetMulticast_ResetTags(TEXT("None"));

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
	UE_LOG(LogClass, Warning, TEXT("누가 나 때림! (HP : %f)"), Damage);
	//if (DamageEvent.IsOfType(FPointDamageEvent::ClassID)) {
		float TempHP = 0;
		// if (DamageCauser Tag == 수리도구) {
		//TempHP = CurrentHP + Damage;
		//}
		// else if (DamageCauser Tag == Bullet && EventInstigator 내가 아니면) {
		TempHP = CurrentHP - Damage;
		UE_LOG(LogClass, Warning, TEXT("누가 나 때림! (HP : %f)"), TempHP);

		// 피격 애니메이션
		//}

		CurrentHP = FMath::Clamp(CurrentHP, 0.0f, 100.0f);

		if (CurrentHP <= 0 && EventInstigator != NULL)
		{
			// 죽음 애니메이션
			// 막타 팀으로 TeamColor 변경
			if (EventInstigator->GetPawn()->ActorHasTag(TEXT("Player"))) {
				//ABattlePC* PC = Cast<ABattlePC*>(EventInstigator->GetPawn());
				UE_LOG(LogClass, Warning, TEXT("from Player"));
			}
			else if (EventInstigator->GetPawn()->ActorHasTag(TEXT("Minion"))) {
				UE_LOG(LogClass, Warning, TEXT("from Minion"));
			}
		}
	//}

	
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
	// HP UI 변경
}

// Blackboard 포함한 State 변경
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
		if (AIC) // CurrentTeam 체크 필요
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
		// Missile 발사
	}
}

void AFairyPawn::EndFire()
{
	bIsEndFire = true;
}


void AFairyPawn::NetMulticast_ResetTags_Implementation(const FName & TowerTag)
{
	if (Tags.Num() <= 0) {
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ETeamColor"), true);

		Tags.Add(TEXT("Tower"));	
		if (enumPtr) Tags.Add(FName(enumPtr->GetNameStringByIndex((int32)TeamColor)));
	}
	else {
		Tags.Empty();
		Tags.Add(TEXT("Tower"));
		Tags.Add(TowerTag);
	}
}



