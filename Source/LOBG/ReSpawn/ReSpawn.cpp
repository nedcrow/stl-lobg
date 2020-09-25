// Fill out your copyright notice in the Description page of Project Settings.


#include "ReSpawn.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

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

