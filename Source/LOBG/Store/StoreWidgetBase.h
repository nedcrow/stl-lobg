// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoreItemWidgetBase.h"
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StoreData")
		TArray<FString> ItemTextArray;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StoreData")
		TArray<UMaterialInstance*> ItemInstanceArray;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StoreData")
		TArray<EItemName> ItemNameArray;

	//TArray<class UStoreItemWidgetBase*> ItemWidgetArray;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StoreData")
		TSubclassOf<class UStoreItemWidgetBase> StoreItemWidgetClass;

	void InitItemArray();

	void SetVisiBilitySlot(ESlateVisibility NewValue);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UScrollBox* ItemBox;
};
