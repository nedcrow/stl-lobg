// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../LOBGGameInstance.h"
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UButton* LobbyBorder;

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

	//TeamSlot
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UScrollBox* RedTeamSlot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UScrollBox* BlueTeamSlot;

	//Slot√ ±‚»≠
	void InitSlot();

	//∫ÛΩΩ∑‘ »Æ¿Œ
	int IsEmptySlot();

	void SplitTeam(const TArray<FString>& RedArray, const TArray<FString>& BlueArrayj, FString MyName);

	//«√∑π¿ÃæÓ¿« ∆¿ªˆ±Ú º≥¡§
	void SetGITeamColor(ETeamColor Color);

	UFUNCTION()
	void BackGroundFunction();

};
