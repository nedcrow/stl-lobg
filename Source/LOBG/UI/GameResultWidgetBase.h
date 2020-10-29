// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "../LOBGGameInstance.h"

#include "GameResultWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API UGameResultWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	class UTextBlock* ResultTitle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	class UScrollBox* MVPScrollBox;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	class UScrollBox* BlueMVP;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	class UScrollBox* RedMVP;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	class UButton* ExitButton;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MI")
	TArray<UMaterialInstance*> BlueMVPArr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MI")
	TArray<UMaterialInstance*> RedMVPArr;

	UFUNCTION()
	void ExitBattle();

	void SetMVP(ETeamColor WinColor);

};
