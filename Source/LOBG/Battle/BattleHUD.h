// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BattleHUD.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API ABattleHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD()override;
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UTexture2D* CrosshairAsset;

};
