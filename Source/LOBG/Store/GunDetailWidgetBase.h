// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GunDetailWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API UGunDetailWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
        class UBorder* GunImage;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
        class UTextBlock* GunName;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
        class UTextBlock* GunDescription;

    virtual void NativeConstruct() override;

    void SetGunImage(UMaterialInstance* NewMaterial);

    void SetGunName(FString ThisGunName);

    void SetGunDescription(FString ThisGunName);
};
