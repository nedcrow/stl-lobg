// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDataTable.generated.h"

//UENUM(BlueprintType)
//enum class EItemType : uint8
//{
//	Unknown = 0 UMETA(Display = "Unknown"),
//	Consume = 1 UMETA(Display = "Consume"),
//	Equip	= 2 UMETA(Display = "Equip"),
//};
//
//UENUM(BlueprintType)
//enum class EItemName : uint8
//{
//	SpeedUp = 0		UMETA(Display = "SpeedUp"),
//	Grip = 1		UMETA(Display = "Grip"),
//	Ironsight = 2		UMETA(Display = "Ironsight"),
//	heel = 3		UMETA(Display = "heel"),
//	bullet = 4		UMETA(Display = "bullet"),
//};

USTRUCT(BlueprintType)
struct LOBG_API FItemDataTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int ItemIndex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString ItemName;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//	EItemName ItemName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int ItemPrice;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString ItemDescription;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TAssetPtr<class UMaterialInstance> ItemImage;
};
