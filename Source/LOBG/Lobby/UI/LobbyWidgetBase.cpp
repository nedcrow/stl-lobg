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
	int FindIndex = IsEmptySlot();
	if (FindIndex == 0)
	{
		ULOBGGameInstance* GI = Cast<ULOBGGameInstance>(GetGameInstance());
		if (GI)
		{
			for (int i = 0; i < NewNames.Num() - 1; ++i)
			{
				if (FindIndex > -1)
				{
					if (FindIndex > 10)
					{
						UTeamSlot* BlueSlot = Cast<UTeamSlot>(BlueTeamSlot->GetChildAt(FindIndex - 11));
						if (BlueSlot && BlueSlot->bUse == false)
						{
							BlueSlot->bUse = true;
							BlueSlot->SetColor(FLinearColor::Blue);
							BlueSlot->SetUserName(NewNames[i]);
							BlueSlot->SetVisibility(ESlateVisibility::Visible);
						}
					}
					else
					{
						UTeamSlot* RedSlot = Cast<UTeamSlot>(RedTeamSlot->GetChildAt(FindIndex));
						if (RedSlot && RedSlot->bUse == false)
						{
							RedSlot->bUse = true;
							RedSlot->SetColor(FLinearColor::Red);
							RedSlot->SetUserName(NewNames[i]);
							RedSlot->SetVisibility(ESlateVisibility::Visible);
						}
					}
				}
				FindIndex = IsEmptySlot();
			}
		}
	}
	if (FindIndex > -1)
	{
		ULOBGGameInstance* GI = Cast<ULOBGGameInstance>(GetGameInstance());
		if (GI)
		{
			if (FindIndex > 10)
			{
				UTeamSlot* BlueSlot = Cast<UTeamSlot>(BlueTeamSlot->GetChildAt(FindIndex - 11));
				if (BlueSlot && BlueSlot->bUse == false)
				{
					BlueSlot->bUse = true;
					BlueSlot->SetColor(FLinearColor::Blue);
					BlueSlot->SetUserName(NewNames.Last());
					BlueSlot->SetVisibility(ESlateVisibility::Visible);

					ALobbyPC* PC = Cast<ALobbyPC>(GetOwningPlayer());
					if (PC && PC->IsLocalPlayerController())
					{
						GI->TeamColor = ETeamColor::Blue;
						UE_LOG(LogClass, Warning, TEXT("TeamColor Blue"));
					}
					
				}
			}
			else
			{
				UTeamSlot* RedSlot = Cast<UTeamSlot>(RedTeamSlot->GetChildAt(FindIndex));
				if (RedSlot && RedSlot->bUse == false)
				{
					RedSlot->bUse = true;
					RedSlot->SetColor(FLinearColor::Red);
					RedSlot->SetUserName(NewNames.Last());
					RedSlot->SetVisibility(ESlateVisibility::Visible);

					ALobbyPC* PC = Cast<ALobbyPC>(GetOwningPlayer());
					if (PC && PC->IsLocalPlayerController())
					{
						GI->TeamColor = ETeamColor::Red;
						UE_LOG(LogClass, Warning, TEXT("TeamColor Red"));
					}
					
				}
			}
		}
	}
}

