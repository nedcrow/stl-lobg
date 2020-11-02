// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "ItemDataTable.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Gun = 0 UMETA(Display = "Gun"),
	Upgrade = 1 UMETA(Display = "Upgrade"),
	Consume = 2 UMETA(Display = "Consume"),
};

//UENUM(BlueprintType)
//enum class EItemName : uint8
//{
//	SpeedUp = 0		UMETA(Display = "SpeedUp"),
//	Grip = 1		UMETA(Display = "Grip"),
//	Ironsight = 2		UMETA(Display = "Ironsight"),
//	Heel = 3		UMETA(Display = "heel"),
//	Bullet = 4		UMETA(Display = "bullet"),
//};

USTRUCT(BlueprintType)
struct LOBG_API FItemDataTableStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int ItemIndex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString ItemName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		EItemType ItemType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int ItemPrice;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString ItemDescription;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TAssetPtr<class UMaterialInstance> ItemImage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TAssetPtr<class USkeletalMesh> GunMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int GunAPU1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int GunAPU2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int GunAPU3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int UpgradePriceOne;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int UpgradePriceTwo;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int UpgradePriceThree;
};

UCLASS()
class LOBG_API AItemDataTable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemDataTable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
