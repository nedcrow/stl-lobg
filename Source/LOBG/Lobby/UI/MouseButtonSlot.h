// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MouseButtonSlot.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API UMouseButtonSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
        class UTextBlock* UserName;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
        class UButton* SlotButton;

    virtual void NativeConstruct() override;

    void SetName(FString NewName);
};
