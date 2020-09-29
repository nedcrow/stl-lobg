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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Base property
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Mesh")
	class UStaticMesh* Mesh;
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
};
