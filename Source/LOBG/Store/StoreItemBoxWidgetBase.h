// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Data/ItemDataTable.h"
#include "StoreItemBoxWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API UStoreItemBoxWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UButton* GunSlotButton;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UButton* UpgradeSlotButton;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UButton* ConsumeSlotButton;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UScrollBox* GunBox;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UScrollBox* UpgradeBox;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UScrollBox* ConsumeBox;

	virtual void NativeConstruct() override;

	UFUNCTION()
		void ClickGunButton();

	UFUNCTION()
		void ClickUpgradeButton();

	UFUNCTION()
		void ClickConsumeButton();

	void InitSlot(class UScrollBox* NewScrollBox, EItemType MyItemType, int AllSlotCount);
	void InitAllSlot();

	void CollapsedAllBox();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UDataTable* ItemDataTable;

	void CheckSlotActive(class UScrollBox* NewScrollBox);
	void CheckAllSlotActive();

	void SetVisiblityItemSlot(class UScrollBox* NewScrollBox);

	UFUNCTION(BlueprintCallable)
		FItemDataTableStruct GetItemData(int Index) const;

	void SetUpgradeGunUpdate(FString CurrentGunName, int DataIndex);

	void CheckSleepSlot();

	void WakeUpSlot();
};
