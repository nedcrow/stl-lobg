// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TabSlotWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API UTabSlotWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UImage* PlayerMeshImage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UTextBlock* PlayerName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UPlayerStatWidgetBase* PlayerStatUI;

	virtual void NativeConstruct()override;

	void SetPlayerMesh(UMaterialInstance* PlayerMesh);

	void SetPlayerName(FString UserName);
};
