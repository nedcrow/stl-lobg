// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API UBattleWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widget")
		class UProgressBar* HPBar;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widget")
		class UProgressBar* ExpBar;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widget")
		class UTextBlock* Money;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widget")
		class UTextBlock* Level;

	virtual void NativeConstruct()override;

	void SetHPBar(float Percent);

	void SetExpBar(float Percent);

	void SetMoney(int AddMoney);

	void SetLevel(int AddLevel);
};