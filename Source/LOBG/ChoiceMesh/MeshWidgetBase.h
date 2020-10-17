// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MeshWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API UMeshWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
        class UButton* FemaleButton;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
        class UButton* MaleButton;

    virtual void NativeConstruct() override;

    UFUNCTION()
    void ClickedButton();

    bool bHasMesh = false;
};
