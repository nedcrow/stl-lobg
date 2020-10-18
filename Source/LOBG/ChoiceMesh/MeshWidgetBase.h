// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MeshWidgetBase.generated.h"

UENUM(BlueprintType)
enum class EMeshType : uint8
{
    None =      0 UMETA(Display = "None"),
    Female =    1 UMETA(Display = "Female"),
    Male =      2 UMETA(Display = "Male"),
};

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
    void ClickedFemaleButton();

    UFUNCTION()
        void ClickedMaleButton();

    bool bHasMesh = false;

    EMeshType CurrentMeshType = EMeshType::None;
};
