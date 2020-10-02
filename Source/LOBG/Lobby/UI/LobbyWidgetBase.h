// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API ULobbyWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	// About start button
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UButton* StartGameButton;
	UFUNCTION()
		void OnStartGameButton();
		void HideStartGameButton();

	// About connect count
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UTextBlock* ConnectCount;
		void SetConnectCount(int Value);

	// About chatting
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UChattingWidgetBase* ChattingWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UScrollBox* RedTeamSlot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UScrollBox* BlueTeamSlot;

	void InitSlot();

	void SplitTeam(FString UserID);

	int IsEmptySlot();

	void SetSlot(int SlotIndex, FString UserID);
};
