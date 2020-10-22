// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameStartWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API UGameStartWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UTextBlock* GameStartTime;

	virtual void NativeConstruct()override;

	void SetGameStartTimeText(int LeftTime);
};
