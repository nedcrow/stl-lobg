// Fill out your copyright notice in the Description page of Project Settings.


#include "ReSpawn.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "../Battle/BattlePC.h"
#include "../Battle/BattleCharacter.h"

// Sets default values
AReSpawn::AReSpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USphereComponent>(TEXT("Root"));
	ReSpawnArea = CreateDefaultSubobject<USceneComponent>(TEXT("ReSpawnArea"));
	Sphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));

	RootComponent = Root;
	ReSpawnArea->SetupAttachment(RootComponent);
	Sphere->SetupAttachment(RootComponent);

	Root->OnComponentBeginOverlap.AddDynamic(this, &AReSpawn::BeginOverlapProcess);
	Root->OnComponentEndOverlap.AddDynamic(this, &AReSpawn::EndOverlapProcess);

}

// Called when the game starts or when spawned
void AReSpawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AReSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AReSpawn::BeginOverlapProcess(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult & SweepResult)
{
				UE_LOG(LogClass, Warning, TEXT("bStoreOpen is true"));
	if (OtherActor->ActorHasTag(TEXT("Player")))
	{
		ABattleCharacter* PlayerPawn = Cast<ABattleCharacter>(OtherActor);
		if (PlayerPawn)
		{
			ABattlePC* PC = Cast<ABattlePC>(PlayerPawn->GetController());
			if (PC)
			{
				PC->bStoreOpen = true;
			}
		}
	}
	
}

void AReSpawn::EndOverlapProcess(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	ABattleCharacter* PlayerPawn = Cast<ABattleCharacter>(OtherActor);
	if (PlayerPawn)
	{
		ABattlePC* PC = Cast<ABattlePC>(PlayerPawn->GetController());
		if (PC)
		{
			UE_LOG(LogClass, Warning, TEXT("bStoreOpen is false"));
			PC->bStoreOpen = false;
		}
	}
}

