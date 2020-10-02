// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshesRingComponent.h"

UMeshesRingComponent::UMeshesRingComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UMeshesRingComponent::BeginPlay()
{
	Super::BeginPlay();

	// InstanceMeshes creating & Transform setting.
	float RUnit = 360.f / MeshCount;
	UE_LOG(LogClass, Warning, TEXT("Radius: %f / Unit: %f"), Radius, RUnit);
	for (int i = 0; i < MeshCount; i++) {
		FName MeshName = *FString::Printf(TEXT("Missile_%d"), i); // CreateDefaultSubobject 매개변수 고려하여 FString::Printf로
		float RotateDegree = RUnit * i;
		float x = Radius * (FMath::Cos(RotateDegree * OneDegree));
		float y = Radius * (FMath::Sin(RotateDegree * OneDegree));
		FVector Position = FVector(x, y, 0);
		FRotator Rotation = bIsLookBody ? FRotator(0, RotateDegree, 0) : FRotator::ZeroRotator;
		FVector Scale = FVector::OneVector;
		UE_LOG(LogClass, Warning, TEXT("Missile_%d: FRotator_(%f, %f, %f)"), i, Position.X, Position.Y, Position.Z);
		UE_LOG(LogClass, Warning, TEXT("Missile_%d: FRotator_(%f, %f, %f)"), i, Rotation.Roll, Rotation.Pitch, Rotation.Yaw);

		AddInstance(FTransform(Rotation, Position, Scale));
	}
}

void UMeshesRingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bIsRotatable) RotateAround(DeltaTime);
}

void UMeshesRingComponent::RotateAround(float DeltaTime)
{
	FRotator TempRotation = FRotator(0, DeltaTime * Speed, 0);
	AddRelativeRotation(TempRotation);
}




