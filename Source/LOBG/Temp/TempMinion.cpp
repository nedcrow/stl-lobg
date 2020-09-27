// Fill out your copyright notice in the Description page of Project Settings.


#include "TempMinion.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Battle/BattlePC.h"
#include "../Battle/BattleCharacter.h"

// Sets default values
ATempMinion::ATempMinion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = Sphere;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	Tags.Add(TEXT("Minion"));
}

// Called when the game starts or when spawned
void ATempMinion::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATempMinion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ATempMinion::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageEvent.IsOfType(FDamageEvent::ClassID))
	{
		CurrentHP -= DamageAmount;
		UE_LOG(LogClass, Warning, TEXT("Minion Current HP : %f"), CurrentHP);
	}
	
	if (CurrentHP <= 0)
	{
		ABattlePC* PC = Cast<ABattlePC>(EventInstigator);
		if (PC)
		{
			ABattleCharacter* Pawn = Cast<ABattleCharacter>(PC->GetPawn());
			if (Pawn)
			{
				Pawn->Server_SetBooty(MinionMoney, MinionExp);
			}
		}
		CurrentHP = 20.0f;
	}
	return 0.0f;
}

