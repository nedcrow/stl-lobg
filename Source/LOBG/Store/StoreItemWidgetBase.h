// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoreItemWidgetBase.generated.h"

UENUM(BlueprintType)
enum class EItemName : uint8
{
	AttackPointUp = 0		UMETA(DisplayName = "AttackPointUp"),
	SpeedUp = 1		UMETA(DisplayName = "SpeedUp"),
	FullHP = 2		UMETA(DisplayName = "FullHP"),
};

/**
 * 
 */
UCLASS()
class LOBG_API UStoreItemWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UBorder* ItemBorder;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UButton* ItemButton;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UTextBlock* ItemText;

	EItemName MyItemName;

	virtual void NativeConstruct() override;

	UFUNCTION()
		void ClickedItemButton();

	void SetItemText(FString newText);

	void SetItemBorder(UMaterialInstance* NewMaterial);
};
