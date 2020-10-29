// Fill out your copyright notice in the Description page of Project Settings.


#include "ReSpawn.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "../Battle/BattlePC.h"
#include "../Battle/BattleCharacter.h"

// Sets default values
AReSpawn::AReSpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetBoxExtent(FVector(250.f, 200.f, 132.f));
	RootComponent = Box;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(RootComponent);
	Body->SetRelativeLocation(FVector(-87.f, 0, -100.f));

	ReSpawnArea = CreateDefaultSubobject<USceneComponent>(TEXT("ReSpawnArea"));
	ReSpawnArea->SetupAttachment(Body);
	//ReSpawnArea->SetRelativeLocation(FVector())

	Box->OnComponentBeginOverlap.AddDynamic(this, &AReSpawn::BeginOverlapProcess);
	Box->OnComponentEndOverlap.AddDynamic(this, &AReSpawn::EndOverlapProcess);

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
	if (OtherActor->ActorHasTag(TEXT("Player")))
	{
		ABattleCharacter* PlayerPawn = Cast<ABattleCharacter>(OtherActor);
		if (PlayerPawn)
		{
			if (HasAuthority())
			{
				PlayerPawn->Server_FullHP();
			}

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
			PC->bStoreOpen = false;
		}
	}
}

