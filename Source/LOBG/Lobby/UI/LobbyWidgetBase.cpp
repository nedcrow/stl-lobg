// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidgetBase.h"
#include "../LobbyGM.h"
#include "../LobbyPC.h"
#include "ChattingWidgetBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void ULobbyWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	StartGameButton = Cast<UButton>(GetWidgetFromName(TEXT("StartGameButton")));
	ConnectCount = Cast<UTextBlock>(GetWidgetFromName(TEXT("ConnectCount")));
	ChattingWidget = Cast<UChattingWidgetBase>(GetWidgetFromName(TEXT("ChattingWidget")));


	if (StartGameButton) {
		StartGameButton->OnClicked.AddDynamic(this, &ULobbyWidgetBase::OnStartGameButton);
	}

	if (ConnectCount) {
		ConnectCount->SetText(FText::FromString("1"));
	}
}

void ULobbyWidgetBase::OnStartGameButton()
{
	ALobbyGM* GM = Cast<ALobbyGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM) {
		GM->StartGame();
	}
} // 게임 시작은 방장의 권한

void ULobbyWidgetBase::HideStartGameButton()
{
	if (StartGameButton) {
		StartGameButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ULobbyWidgetBase::SetConnectCount(int Value)
{
	FString Temp = FString::Printf(TEXT("%d"), Value);
	if (ConnectCount) {
		ConnectCount->SetText(FText::FromString(Temp));
	}
}

