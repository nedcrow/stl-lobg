// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TeamSlot.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API UTeamSlot : public UUserWidget
{
	GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
        class UBorder* TeamColor;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
        class UTextBlock* UserName;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
        class UButton* SlotButton;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        class UMouseButtonSlot* MouseButtonSlot;

    virtual void NativeConstruct() override;

    void SetColor(FLinearColor NewColor);
    void SetUserName(FString NewName);

    bool bUse = false;

    UFUNCTION()
    void HoveredSlotButton();

    UFUNCTION()
        void ClickedChangeTeam();
};
