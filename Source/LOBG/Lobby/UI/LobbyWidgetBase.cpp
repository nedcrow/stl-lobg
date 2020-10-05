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
} // ���� ������ ������ ����

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
	//���� �ʱ�ȭ
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
	//�÷��̾��� ���̵���� �� �ִ� �迭�� �����ͼ� ���Կ� �Ҵ�

	ULOBGGameInstance* GI = Cast<ULOBGGameInstance>(GetGameInstance());
	if (GI)
	{
		int FindIndex = IsEmptySlot();
		//FindIndex�� 0�̶�� ���� ���� �� ���� �÷��̾��� ���̴�
		//���ݱ��� �������ִ� �÷��̾���� ���� ���Կ� �־���ϹǷ�
		//NewNames�� ��ȸ�ϸ� ���Կ� �־��ش�.
		if (FindIndex == 0)
		{
			for (int i = 0; i < NewNames.Num() - 1; ++i)
			{
				SetSlot(NewNames[i], FindIndex);
				FindIndex = IsEmptySlot();
			}
		}

		//���Կ� ������ �÷��̾���� �ְ� �ڱ� �ڽ��� �־��ش�.
		//GI�� TeamColor�� ����
		ETeamColor SlotColor = SetSlot(NewNames.Last(), FindIndex);

		//�ٸ� Ŭ���̾�Ʈ���� �߰��� �÷��̾� ������ ����ȭ�ϸ鼭 TeamColor��
		//�����Ű�� �ʰ� None���� �˻�
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

