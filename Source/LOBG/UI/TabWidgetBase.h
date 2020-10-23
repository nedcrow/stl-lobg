// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TabWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API UTabWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UScrollBox* RedScrollBox;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UScrollBox* BlueScrollBox;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TabUI")
		UMaterialInstance* FemaleImage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TabUI")
		UMaterialInstance* MaleImage;

	virtual void NativeConstruct()override;

	void InitSlot();

	void UpdateSlot();
};
