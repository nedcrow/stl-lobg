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

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
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
	int LimmitCount;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Speed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Radius;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 bIsPitch:1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 bIsYaw : 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 bIsRoll : 1;
};
