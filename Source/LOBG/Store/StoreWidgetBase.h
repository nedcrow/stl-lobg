// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Data/ItemDataTable.h"
#include "StoreWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API UStoreWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UBorder* StoreBorder;

	void SetVisiBilitySlot(ESlateVisibility NewValue);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UStoreItemBoxWidgetBase* ItemBox;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UDataTable* ItemDataTable;

	UFUNCTION(BlueprintCallable)
		FItemDataTableStruct GetItemData(int Index) const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UGunDetailWidgetBase* StoreGunDetail;
};
