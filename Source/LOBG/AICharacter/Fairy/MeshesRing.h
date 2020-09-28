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

	class USceneComponent* MeshesRing;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Base property
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		TArray<UStaticMeshComponent*> Meshes;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UStaticMesh* Mesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int MeshCount;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Speed = 100.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Radius = 32.f;

	// About rotate
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		uint8 bIsRotatable : 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		uint8 bIsLookBody : 1;

	void RotateAround(float DeltaTime);
};
