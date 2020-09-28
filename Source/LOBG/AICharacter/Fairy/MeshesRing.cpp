// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshesRing.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AMeshesRing::AMeshesRing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshesRing = CreateDefaultSubobject<USceneComponent>(TEXT("MeshesRing"));
	MeshesRing->SetupAttachment(RootComponent);

	float unit = 360.f / MeshCount;

	// StaticMeshes creating & Transfom setting.
	for (int i = 0; i < MeshCount; i++) {
		FName MeshName = *FString::Printf(TEXT("Missile_%d"), i); // CreateDefaultSubobject 매개변수 고려하여 FString::Printf로
		UStaticMeshComponent* TempMesh = CreateDefaultSubobject<UStaticMeshComponent>(MeshName);
		TempMesh->SetStaticMesh(Mesh);
		TempMesh->SetupAttachment(MeshesRing);
		TempMesh->SetRelativeLocation(FVector(Radius, 0, 0));
		TempMesh->SetVisibility(true);
		if (bIsRotatable) { TempMesh->SetRelativeRotation(FRotator(0, unit * i, 0)); }
		MeshesRing->AddRelativeRotation(FRotator(0, unit * i, 0));
		Meshes.Add(TempMesh);
		UE_LOG(LogClass, Warning, TEXT("Test_%d"), i);
	}

}

// Called when the game starts or when spawned
void AMeshesRing::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMeshesRing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsRotatable) RotateAround(DeltaTime);
}

void AMeshesRing::RotateAround(float DeltaTime)
{
	FRotator TempRotation = FRotator(0, DeltaTime * Speed, 0);
	MeshesRing->AddRelativeRotation(TempRotation);
}