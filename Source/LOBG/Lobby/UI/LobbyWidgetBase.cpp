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

void ULobbyWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	StartGameButton = Cast<UButton>(GetWidgetFromName(TEXT("StartGameButton")));
	ConnectCount = Cast<UTextBlock>(GetWidgetFromName(TEXT("ConnectCount")));
	ChattingWidget = Cast<UChattingWidgetBase>(GetWidgetFromName(TEXT("ChattingWidget")));
	RedTeamSlot = Cast<UScrollBox>(GetWidgetFromName(TEXT("RedTeamSlot")));
	BlueTeamSlot = Cast<UScrollBox>(GetWidgetFromName(TEXT("BlueTeamSlot")));

	if (StartGameButton)
	{
		StartGameButton->OnClicked.AddDynamic(this, &ULobbyWidgetBase::OnStartGameButton);
	}

	if (ConnectCount)
	{
		ConnectCount->SetText(FText::FromString("1"));
	}

	InitSlot();
}

void ULobbyWidgetBase::OnStartGameButton()
{
	ALobbyGM* GM = Cast<ALobbyGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		GM->StartGame();
	}
} // 게임 시작은 방장의 권한

void ULobbyWidgetBase::HideStartGameButton()
{
	if (StartGameButton)
	{
		StartGameButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ULobbyWidgetBase::SetConnectCount(int Value)
{
	FString Temp = FString::Printf(TEXT("%d"), Value);
	if (ConnectCount)
	{
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

void ULobbyWidgetBase::SplitTeam(const TArray<FString>& NewNames)
{
	//플레이어의 아이디들이 들어가 있는 배열을 가져와서 슬롯에 할당

	ULOBGGameInstance* GI = Cast<ULOBGGameInstance>(GetGameInstance());
	if (GI)
	{
		int FindIndex = IsEmptySlot();
		//FindIndex가 0이라는 뜻은 이제 막 들어온 플레이어라는 뜻이다
		//지금까지 접속해있는 플레이어들을 먼저 슬롯에 넣어야하므로
		//NewNames를 순회하며 슬롯에 넣어준다.
		if (FindIndex == 0)
		{
			for (int i = 0; i < NewNames.Num() - 1; ++i)
			{
				SetSlot(NewNames[i], FindIndex);
				FindIndex = IsEmptySlot();
			}
		}

		//슬롯에 기존의 플레이어들을 넣고 자기 자신을 넣어준다.
		//GI의 TeamColor도 변경
		ETeamColor SlotColor = SetSlot(NewNames.Last(), FindIndex);

		//다른 클라이언트에서 추가된 플레이어 슬롯을 동기화하면서 TeamColor를
		//변경시키지 않게 None인지 검사
		if (GI->TeamColor == ETeamColor::None)
		{
			SetGITeamColor(SlotColor);
		}
	}
}

ETeamColor ULobbyWidgetBase::SetSlot(FString UserName, int Index)
{
	if (Index > -1)
	{
		if (Index > 10)
		{
			UTeamSlot* BlueSlot = Cast<UTeamSlot>(BlueTeamSlot->GetChildAt(Index - 11));
			if (BlueSlot && BlueSlot->bUse == false)
			{
				BlueSlot->bUse = true;
				BlueSlot->SetColor(FLinearColor::Blue);
				BlueSlot->SetUserName(UserName);
				BlueSlot->SetVisibility(ESlateVisibility::Visible);
				return ETeamColor::Blue;
			}
		}
		else
		{
			UTeamSlot* RedSlot = Cast<UTeamSlot>(RedTeamSlot->GetChildAt(Index));
			if (RedSlot && RedSlot->bUse == false)
			{
				RedSlot->bUse = true;
				RedSlot->SetColor(FLinearColor::Red);
				RedSlot->SetUserName(UserName);
				RedSlot->SetVisibility(ESlateVisibility::Visible);
				return ETeamColor::Red;
			}
		}
	}
	return ETeamColor::None;
}

void ULobbyWidgetBase::SetGITeamColor(ETeamColor Color)
{
	ULOBGGameInstance* GI = Cast<ULOBGGameInstance>(GetGameInstance());
	if (GI)
	{
		switch (Color)
		{
		case ETeamColor::Red:
			GI->TeamColor = ETeamColor::Red;
			UE_LOG(LogClass, Warning, TEXT("RED"));
			break;

		case ETeamColor::Blue:
			GI->TeamColor = ETeamColor::Blue;
			UE_LOG(LogClass, Warning, TEXT("Blue"));
			break;
		default:
			break;
		}
	}
}

