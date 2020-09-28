// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshesRingComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values for this component's properties
UMeshesRingComponent::UMeshesRingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<USceneComponent>(TEXT("Body"));
	float unit = 360/LimmitCount;

	for (int i = 0; i < LimmitCount; i++) {
		UStaticMeshComponent* TempMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Missile_&d"), i);
		TempMesh->SetStaticMesh(Mesh);
		TempMesh->SetupAttachment(Body);
		TempMesh->SetRelativeLocation(FVector(Radius, 0, 0));
		TempMesh->SetRelativeRotation(FRotator(0, unit*i, 0));
		Meshes.Add(TempMesh);
	}
}


// Called when the game starts
void UMeshesRingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMeshesRingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FRotator TempRotation = FRotator(
		bIsPitch ? DeltaTime * Speed : 0,
		bIsYaw ? DeltaTime * Speed : 0,
		bIsRoll ? DeltaTime * Speed : 0
	);
	Body->AddRelativeRotation(TempRotation);
}

