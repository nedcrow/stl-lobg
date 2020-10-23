// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API UPlayerStatWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UTextBlock* AttackStat;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UTextBlock* SpeedStat;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UTextBlock* MoneyStat;

	virtual void NativeConstruct()override;

	void SetStatText(int AttackPoint, int SpeedPoint, int MoneyPoint);
};
