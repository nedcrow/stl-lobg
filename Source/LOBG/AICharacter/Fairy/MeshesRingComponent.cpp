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

	float unit = 360.f / MeshCount;

	// StaticMeshes creating & Transfom setting.
	for (int i = 0; i < MeshCount; i++) {
		FName MeshName = *FString::Printf(TEXT("Missile_%d"), i); // CreateDefaultSubobject 매개변수 고려하여 FString::Printf로
		UStaticMeshComponent* TempMesh = CreateDefaultSubobject<UStaticMeshComponent>(MeshName);
		TempMesh->SetStaticMesh(Mesh);
		TempMesh->SetupAttachment(Body);
		TempMesh->SetRelativeLocation(FVector(Radius, 0, 0));
		TempMesh->SetVisibility(true);
		if (bIsRotatable) TempMesh->SetRelativeRotation(FRotator(0, unit*i, 0));
		Body->AddRelativeRotation(FRotator(0, unit * i, 0));
		Meshes.Add(TempMesh);
		UE_LOG(LogClass,Warning,TEXT("Test_%d"),i);
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
	if(bIsRotatable) RotateAround(DeltaTime);
	
}

void UMeshesRingComponent::RotateAround(float DeltaTime)
{
	FRotator TempRotation = FRotator(0, DeltaTime * Speed, 0);
	Body->AddRelativeRotation(TempRotation);
}

