// Fill out your copyright notice in the Description page of Project Settings.


#include "TempTower.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "../Battle/BattleGM.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATempTower::ATempTower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = Sphere;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	Tags.Add(TEXT("Tower"));
}

// Called when the game starts or when spawned
void ATempTower::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATempTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ATempTower::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageEvent.IsOfType(FDamageEvent::ClassID))
	{
		TowerHP -= DamageAmount;
	}

	if (TowerHP <= 0)
	{
		ABattleGM* GM = Cast<ABattleGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GM)
		{
			//GM->CountTower();
		}
		Destroy();
	}
	return 0.0f;
}

