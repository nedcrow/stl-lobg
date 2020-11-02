// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Data/ItemDataTable.h"
#include "StoreItemWidgetBase.generated.h"

//UENUM(BlueprintType)
//enum class EItemType : uint8
//{
//	Unknown = 0 UMETA(Display = "Unknown"),
//	Consume = 1 UMETA(Display = "Consume"),
//	Equip = 2 UMETA(Display = "Equip"),
//};
//
UENUM(BlueprintType)
enum class EItemName : uint8
{
	SpeedUp = 0		UMETA(Display = "SpeedUp"),
	Grip = 1		UMETA(Display = "Grip"),
	Ironsight = 2		UMETA(Display = "Ironsight"),
	Heel = 3		UMETA(Display = "heel"),
	Bullet = 4		UMETA(Display = "bullet"),
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

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	//	class UTextBlock* ItemText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UTextBlock* ItemMoney;

	FString MyItemName;

	int MyItemIndex;

	int MyItemMoney;

	virtual void NativeConstruct() override;

	UFUNCTION()
		void ClickedItemButton();

	void SetItemText(FString newText);

	void SetItemBorder(UMaterialInstance* NewMaterial);

	void SetItemMoney(int newMoney);

	bool InitSlotByMoney();

	bool bEnoughMoney = false;

	UFUNCTION()
	void HoveredItemButton();

	UMaterialInstance* MyItemImage;

	FString MyItemDescription;

	bool bIsSleep = false;
};
