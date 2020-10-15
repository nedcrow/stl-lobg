// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "Battle_PCM.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API ABattle_PCM : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	virtual void UpdateCamera(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Camera")
		float TargetFOV;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
		float NormalFOV = 90.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
		float IronsightFOV = 30.0f;
};
