// Fill out your copyright notice in the Description page of Project Settings.


#include "TabWidgetBase.h"
#include "Components/ScrollBox.h"
#include "TabSlotWidgetBase.h"
#include "Kismet/GameplayStatics.h"
#include "../Battle/BattleGS.h"
#include "PlayerStatWidgetBase.h"

void UTabWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	RedScrollBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("RedScrollBox")));
	BlueScrollBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("BlueScrollBox")));

	InitSlot();
}

void UTabWidgetBase::InitSlot()
{
	for (int i = 0; i < RedScrollBox->GetChildrenCount(); ++i)
	{
		UTabSlotWidgetBase* RedSlot = Cast<UTabSlotWidgetBase>(RedScrollBox->GetChildAt(i));
		if (RedSlot)
		{
			RedSlot->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	for (int i = 0; i < BlueScrollBox->GetChildrenCount(); ++i)
	{
		UTabSlotWidgetBase* BlueSlot = Cast<UTabSlotWidgetBase>(BlueScrollBox->GetChildAt(i));
		if (BlueSlot)
		{
			BlueSlot->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UTabWidgetBase::UpdateSlot()
{
	ABattleGS* GS = Cast<ABattleGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (GS)
	{
		for (int i = 0; i < GS->RedTabDataArray.Num(); ++i)
		{
			UTabSlotWidgetBase* RedSlot = Cast<UTabSlotWidgetBase>(RedScrollBox->GetChildAt(i));
			if (RedSlot)
			{
				switch (GS->RedTabDataArray[i].PlayerMesh)
				{
				case EMeshType::Female:
					RedSlot->SetPlayerMesh(FemaleImage);
					break;

				case EMeshType::Male:
					RedSlot->SetPlayerMesh(MaleImage);
					break;

				case EMeshType::None:
					break;
				}
				
				RedSlot->SetPlayerName(GS->RedTabDataArray[i].PlayerName);
				RedSlot->PlayerStatUI->SetStatText(GS->RedTabDataArray[i].PlayerAttackData, GS->RedTabDataArray[i].PlayerSpeedData, GS->RedTabDataArray[i].PlayerMoneyData);
				RedSlot->SetVisibility(ESlateVisibility::Visible);
			}
		}

		for (int i = 0; i < GS->BlueTabDataArray.Num(); ++i)
		{
			UTabSlotWidgetBase* BlueSlot = Cast<UTabSlotWidgetBase>(BlueScrollBox->GetChildAt(i));
			if (BlueSlot)
			{
				switch (GS->BlueTabDataArray[i].PlayerMesh)
				{
				case EMeshType::Female:
					BlueSlot->SetPlayerMesh(FemaleImage);
					break;

				case EMeshType::Male:
					BlueSlot->SetPlayerMesh(MaleImage);
					break;

				case EMeshType::None:
					break;
				}

				BlueSlot->SetPlayerName(GS->BlueTabDataArray[i].PlayerName);
				BlueSlot->PlayerStatUI->SetStatText(GS->BlueTabDataArray[i].PlayerAttackData, GS->BlueTabDataArray[i].PlayerSpeedData, GS->BlueTabDataArray[i].PlayerMoneyData);
				BlueSlot->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}

}
