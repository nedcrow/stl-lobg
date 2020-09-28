// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MeshesRingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LOBG_API UMeshesRingComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMeshesRingComponent();

	class USceneComponent* Body;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

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
