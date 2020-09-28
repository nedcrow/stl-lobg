// Fill out your copyright notice in the Description page of Project Settings.


#include "MissileHub.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AMissileHub::AMissileHub()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	for (int i = 0; i < LimmitCount; i++) {
		UStaticMeshComponent* Missile = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Missile_&d"), i);
		Missile->SetStaticMesh(MissileMesh);
		MissileMeshes.Add(Missile);
	}
}

// Called when the game starts or when spawned
void AMissileHub::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMissileHub::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

