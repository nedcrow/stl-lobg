// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidgetBase.h"
#include "../LobbyGM.h"
#include "../LobbyPC.h"
#include "../LobbyGS.h"
#include "ChattingWidgetBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "TeamSlot.h"
#include "../../LOBGGameInstance.h"

void ULobbyWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	StartGameButton = Cast<UButton>(GetWidgetFromName(TEXT("StartGameButton")));
	ConnectCount = Cast<UTextBlock>(GetWidgetFromName(TEXT("ConnectCount")));
	ChattingWidget = Cast<UChattingWidgetBase>(GetWidgetFromName(TEXT("ChattingWidget")));
	RedTeamSlot = Cast<UScrollBox>(GetWidgetFromName(TEXT("RedTeamSlot")));
	BlueTeamSlot = Cast<UScrollBox>(GetWidgetFromName(TEXT("BlueTeamSlot")));

	if (StartGameButton) {
		StartGameButton->OnClicked.AddDynamic(this, &ULobbyWidgetBase::OnStartGameButton);
	}

	if (ConnectCount) {
		ConnectCount->SetText(FText::FromString("1"));
	}

	InitSlot();
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

void ULobbyWidgetBase::InitSlot()
{
	//슬롯 초기화
	for (int i = 0; i < RedTeamSlot->GetChildrenCount(); ++i)
	{
		UTeamSlot* RedSlot = Cast<UTeamSlot>(RedTeamSlot->GetChildAt(i));
		if (RedSlot)
		{
			RedSlot->bUse = false;
			RedSlot->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	for (int i = 0; i < BlueTeamSlot->GetChildrenCount(); ++i)
	{
		UTeamSlot* BlueSlot = Cast<UTeamSlot>(BlueTeamSlot->GetChildAt(i));
		if (BlueSlot)
		{
			BlueSlot->bUse = false;
			BlueSlot->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void ULobbyWidgetBase::SplitTeam(FString UserID)
{
	//팀 나누기
	ALobbyPC* PC = GetOwningPlayer<ALobbyPC>();
	if (PC)
	{
		int FindIndex = IsEmptySlot();
		if (FindIndex > -1)
		{
			SetSlot(FindIndex, UserID);
		}
	}
}

int ULobbyWidgetBase::IsEmptySlot()
{
	for (int i = 0; i < RedTeamSlot->GetChildrenCount(); ++i)
	{
		UTeamSlot* RedSlot = Cast<UTeamSlot>(RedTeamSlot->GetChildAt(i));
		if (RedSlot && RedSlot->bUse == false)
		{
			return i;
		}
	}

	for (int i = 0; i < BlueTeamSlot->GetChildrenCount(); ++i)
	{
		UTeamSlot* BlueSlot = Cast<UTeamSlot>(BlueTeamSlot->GetChildAt(i));
		if (BlueSlot && BlueSlot->bUse == false)
		{
			return i + 11;
		}
	}
	return -1;
}

void ULobbyWidgetBase::SetSlot(int SlotIndex, FString UserID)
{
	ALobbyPC* PC = GetOwningPlayer<ALobbyPC>();
	if (PC)
	{
		ULOBGGameInstance* GI = Cast<ULOBGGameInstance>(GetGameInstance());
		if (GI)
		{
			if (SlotIndex > 10)
			{
				UTeamSlot* BlueSlot = Cast<UTeamSlot>(BlueTeamSlot->GetChildAt(SlotIndex - 11));
				if (BlueSlot && BlueSlot->bUse == false)
				{
					BlueSlot->bUse = true;
					BlueSlot->SetColor(FLinearColor::Blue);
					BlueSlot->SetUserName(UserID);
					GI->TeamColor = ETeamColor::Blue;
					BlueSlot->SetVisibility(ESlateVisibility::Visible);
				}
			}
			else
			{
				UTeamSlot* RedSlot = Cast<UTeamSlot>(RedTeamSlot->GetChildAt(SlotIndex));
				if (RedSlot && RedSlot->bUse == false)
				{
					RedSlot->bUse = true;
					RedSlot->SetColor(FLinearColor::Red);
					RedSlot->SetUserName(UserID);
					GI->TeamColor = ETeamColor::Red;
					RedSlot->SetVisibility(ESlateVisibility::Visible);
				}
			}
		}
	}
}

