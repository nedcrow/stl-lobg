// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissileHub.generated.h"

UCLASS()
class LOBG_API AMissileHub : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMissileHub();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Base property
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<UStaticMeshComponent*> MissileMeshes;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UStaticMesh* MissileMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int LimmitCount;
};
