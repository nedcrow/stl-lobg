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
#include "Components/Border.h"
#include "TeamSlot.h"
#include "MouseButtonSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"

void ULobbyWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	LobbyBorder = Cast<UButton>(GetWidgetFromName(TEXT("LobbyBackGround")));
	StartGameButton = Cast<UButton>(GetWidgetFromName(TEXT("StartGameButton")));
	ConnectCount = Cast<UTextBlock>(GetWidgetFromName(TEXT("ConnectCount")));
	ChattingWidget = Cast<UChattingWidgetBase>(GetWidgetFromName(TEXT("ChattingWidget")));
	RedTeamSlot = Cast<UScrollBox>(GetWidgetFromName(TEXT("RedTeamSlot")));
	BlueTeamSlot = Cast<UScrollBox>(GetWidgetFromName(TEXT("BlueTeamSlot")));
	
	if (LobbyBorder)
	{
		LobbyBorder->OnClicked.AddDynamic(this, &ULobbyWidgetBase::BackGroundFunction);
	}

	if (StartGameButton)
	{
		if(!GetWorld()->IsServer()) StartGameButton->SetVisibility(ESlateVisibility::Collapsed);
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
	//사용하지 않지만 혹시 몰라서 남겨둠
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

void ULobbyWidgetBase::SplitTeam(const TArray<FString>& RedArray, const TArray<FString>& BlueArray, FString MyName)
{
	InitSlot();

	for (int i = 0; i < RedArray.Num(); ++i)
	{
		UTeamSlot* RedSlot = Cast<UTeamSlot>(RedTeamSlot->GetChildAt(i));
		RedSlot->bUse = true;
		if (RedArray[i] == MyName)
		{
			RedSlot->SetColor(FLinearColor::Green);
		}
		else
		{
			RedSlot->SetColor(FLinearColor::Red);
		}
		RedSlot->SetUserName(RedArray[i]);
		RedSlot->SetVisibility(ESlateVisibility::Visible);
		RedSlot->MouseButtonSlot->SetName(TEXT("Blue"));
	}

	for (int i = 0; i < BlueArray.Num(); ++i)
	{
		UTeamSlot* BlueSlot = Cast<UTeamSlot>(BlueTeamSlot->GetChildAt(i));
		BlueSlot->bUse = true;
		if (BlueArray[i] == MyName)
		{
			BlueSlot->SetColor(FLinearColor::Green);
		}
		else
		{
			BlueSlot->SetColor(FLinearColor::Blue);
		}
		BlueSlot->SetUserName(BlueArray[i]);
		BlueSlot->SetVisibility(ESlateVisibility::Visible);
		BlueSlot->MouseButtonSlot->SetName(TEXT("Red"));
	}
}

void ULobbyWidgetBase::SetGITeamColor(ETeamColor Color)
{
	//사용하지 않지만 혹시 몰라서 남겨둠
	ULOBGGameInstance* GI = Cast<ULOBGGameInstance>(GetGameInstance());
	if (GI)
	{
		switch (Color)
		{
		case ETeamColor::Red:
			GI->TeamColor = ETeamColor::Red;
			break;

		case ETeamColor::Blue:
			GI->TeamColor = ETeamColor::Blue;
			break;
		default:
			break;
		}
	}
}

void ULobbyWidgetBase::BackGroundFunction()
{
	UE_LOG(LogClass, Warning, TEXT("BackGroundFunction"));
}

