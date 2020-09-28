// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MissileHubComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LOBG_API UMissileHubComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMissileHubComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Base property
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<UStaticMeshComponent*> MissileMeshes;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UStaticMesh* MissileMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int LimmitCount;
		
};
