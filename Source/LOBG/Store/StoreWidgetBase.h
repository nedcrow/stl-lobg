// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoreWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API UStoreWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	//	class UBorder* RedItem;
	//
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	//	class UButton* RedButton;
	//
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	//	class UBorder* BlueItem;
	//
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	//	class UButton* BlueButton;
	//
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	//	class UBorder* GreenItem;
	//
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	//	class UButton* GreenButton;
	//
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	//	class UBorder* YellowItem;
	//
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	//	class UButton* YellowButton;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UVerticalBox* ItemBox;

	virtual void NativeConstruct() override;

	void InitBox();
};
