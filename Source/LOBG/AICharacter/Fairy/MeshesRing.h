// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeshesRing.generated.h"

UCLASS()
class LOBG_API AMeshesRing : public AActor
{
	GENERATED_BODY() 
	
public:	
	// Sets default values for this actor's properties
	AMeshesRing();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class USphereComponent* SphereCollision;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class USceneComponent* InnerCore;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Base property
	float OneDegree = 0.01745; // = pi / 180 radian
	float OneRadian = 57.2958; // = 180 / pi degree
	FTransform* TempTransform;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	class UInstancedStaticMeshComponent* IncetancedMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	int MeshCount = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	float Radius = 32.f;

	// About rotate
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rotation")
	float Speed = 100.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rotation")
	uint8 bIsRotatable : 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rotation")
	uint8 bIsLookBody : 1;

	void RotateAround(float DeltaTime);
};
