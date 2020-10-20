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
		AddRelativeRotation(FRotator(0, DeltaTime*Speed, 0));
	}
}

void UMeshesRingComponent::NetMulticast_RotateAround_Implementation(FRotator NewRotator)
{
	SetRelativeRotation(NewRotator);
	UE_LOG(LogTemp,Warning,TEXT("Rotator:: %f, %f, %f"), NewRotator.Roll, GetRelativeRotation().Yaw, NewRotator.Pitch);
}

void UMeshesRingComponent::StartRotateAround(FTimerHandle TimerHandle)
{
	//GetWorldTimerManager().SetTimer(TimerHandle, this, &AFairyPawn::Server_SetRingComponentRotation, 1.f, false);
}

// Remove instance from last order
void UMeshesRingComponent::NetMulticast_RemoveOne_Implementation()
{
	if (GetInstanceCount() > 0) {
		RemoveInstance(GetInstanceCount()-1);
	}
}

// Add instance from first order
void UMeshesRingComponent::NetMulticast_AddOne_Implementation()
{
	if (GetInstanceCount() < MaxMeshCount) {
		AddInstance(SpawnTransforms[GetInstanceCount()]);
	}
}



