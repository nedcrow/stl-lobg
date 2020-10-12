// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoreItemBase.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API UStoreItemBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UBorder* ItemBorder;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UButton* ItemButton;

	virtual void NativeConstruct() override;

	void SetItemColor(FLinearColor newColor);

	UFUNCTION()
		void ClickItemSlot();
};
