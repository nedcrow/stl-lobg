// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshesRingComponent.h"
#include "GameFramework/Actor.h"

UMeshesRingComponent::UMeshesRingComponent() {
	PrimaryComponentTick.bCanEverTick = true;

	ComponentTags.Add("Missile");
}

void UMeshesRingComponent::BeginPlay()
{
	Super::BeginPlay();

	// InstanceMeshes creating & Transform setting.
	MaxMeshCount = MaxMeshCount > VertexCount ? VertexCount : FMath::Abs(MaxMeshCount + 0);
	float RUnit = 360.f / VertexCount;
	for (int i = 0; i < VertexCount; i++) {
		FName MeshName = *FString::Printf(TEXT("Missile_%d"), i); // CreateDefaultSubobject 매개변수 고려하여 FString::Printf로
		float RotateDegree = RUnit * i;
		float x = Radius * (FMath::Cos(RotateDegree * OneDegree));
		float y = Radius * (FMath::Sin(RotateDegree * OneDegree));
		FVector Position = FVector(x, y, 0);
		FRotator Rotation = bIsLookBody ? FRotator(0, RotateDegree, 0) : FRotator::ZeroRotator;
		Rotation += MeshRotation;
		FVector Scale = FVector::OneVector * StartScale;
		//UE_LOG(LogClass, Warning, TEXT("Missile_%d: FRotator_(%f, %f, %f)"), i, Position.X, Position.Y, Position.Z);
		//UE_LOG(LogClass, Warning, TEXT("Missile_%d: FRotator_(%f, %f, %f)"), i, Rotation.Roll, Rotation.Pitch, Rotation.Yaw);

		if (i < MaxMeshCount)AddInstance(FTransform(Rotation, Position, Scale));
		SpawnTransforms.Add(FTransform(Rotation, Position, Scale));
	}
}

void UMeshesRingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bIsRotatable) {
		AddRelativeRotation(FRotator(0, DeltaTime * Speed, 0));
	}
}

void UMeshesRingComponent::NetMulticast_StartRotateAround_Implementation()
{
	bIsRotatable = true;
}

// Remove instance from last order
void UMeshesRingComponent::NetMulticast_RemoveOne_Implementation(int Index)
{
	if (GetInstanceCount() > 0) {
		RemoveInstance(Index);
	}
}

// Add instance & swap new transform and old transform
void UMeshesRingComponent::NetMulticast_AddOne_Implementation(int Index, FTransform NewTransform)
{
	if (GetInstanceCount() < MaxMeshCount) {
		AddInstance(NewTransform);
		UE_LOG(LogTemp, Warning, TEXT("Spawn location : %d (%f, %f, %f)"), Index, NewTransform.GetLocation().X, NewTransform.GetLocation().Y, NewTransform.GetLocation().Z);

		if (Index+1 != GetInstanceCount()) { // Compare index and updated count
			if (SpawnTransforms[Index].GetLocation() != SpawnTransforms[GetInstanceCount() - 1].GetLocation()) { // same location checking
				SpawnTransforms.Swap(Index, GetInstanceCount() - 1); // Because AddInstance() is not addition to place of index
				UE_LOG(LogTemp, Warning, TEXT("Swap"));
			}
		}
	}
}

// swap new transform and old transform
void UMeshesRingComponent::NetMulticast_SwapTransformWithLastIndex_Implementation(int Index, FTransform NewTransform)
{
	if (GetInstanceCount() < MaxMeshCount) {
		SpawnTransforms.Swap(Index, GetInstanceCount()); // Because AddInstance() is not addition to place of index
		UE_LOG(LogTemp, Warning, TEXT("swap : %d (%f, %f, %f)"), Index, SpawnTransforms[Index].GetLocation().X, SpawnTransforms[Index].GetLocation().Y, SpawnTransforms[Index].GetLocation().Z);
	}
}

TArray<FTransform> UMeshesRingComponent::NetMulticast_GetSpawnTransforms_Implementation()
{
	return SpawnTransforms;
}



