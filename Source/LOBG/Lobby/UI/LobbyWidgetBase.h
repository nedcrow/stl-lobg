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

	//SlotÃÊ±âÈ­
	void InitSlot();

	//ºó½½·Ô È®ÀÎ
	int IsEmptySlot();

	//ÆÀ³ª´©±â
	void SplitTeam(const TArray<FString>& NewNames);

	//½½·Ô¼³Á¤
	ETeamColor SetSlot(FString UserName, int Index);

	//ÇÃ·¹ÀÌ¾îÀÇ ÆÀ»ö±ò ¼³Á¤
	void SetGITeamColor(ETeamColor Color);
};
