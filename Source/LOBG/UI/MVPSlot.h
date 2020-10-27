// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MVPSlot.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API UMVPSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	class UImage* MVPIcon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	class UTextBlock* UserIDText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	class UTextBlock* UserLVText;
	
};
