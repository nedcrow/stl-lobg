// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "MeshesRingComponent.generated.h"

/**
 *
 */
UCLASS()
class LOBG_API UMeshesRingComponent : public UInstancedStaticMeshComponent
{
	GENERATED_BODY()
public:
	UMeshesRingComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// Base property
	float OneDegree = 0.01745; // = pi / 180 radian
	float OneRadian = 57.2958; // = 180 / pi degree

	TArray<FTransform> SpawnTransforms;
	TArray<FTransform> SpawnWorldTransforms;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
		FRotator MeshRotation = FRotator::ZeroRotator;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
		int VertexCount = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
		int MaxMeshCount = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
		float Radius = 32.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
		float StartScale = 1.f; 

	// About rotate
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rotation")
		float Speed = 100.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rotation")
		uint8 bIsRotatable : 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rotation")
		uint8 bIsLookBody : 1;

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_StartRotateAround();
	void NetMulticast_StartRotateAround_Implementation();

	UFUNCTION()
	void RemoveOne();

	UFUNCTION()
	void AddOne(FTransform NewTransform);
};
