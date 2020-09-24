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

	//CrossHair를 로드하는 함수 BeginPlay에서 실행
	UFUNCTION()
	void LoadHUDAsset();

	//그릴 crosshair
	class UTexture2D* CrossAsset;

	//텍스트 담는곳
	FSoftObjectPath T_CrossAsset;

};
