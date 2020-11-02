// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleWidgetBase.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Border.h"
#include "../Lobby/UI/ChattingWidgetBase.h"
#include "Kismet/KismetTextLibrary.h"

void UBattleWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	Money = Cast<UTextBlock>(GetWidgetFromName(TEXT("MoneyText")));
	Level = Cast<UTextBlock>(GetWidgetFromName(TEXT("LevelText")));
	CountPlayer = Cast<UTextBlock>(GetWidgetFromName(TEXT("CountPlayerText")));
	HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HPBar")));
	ExpBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ExpBar")));
	PotionSlot = Cast<UBorder>(GetWidgetFromName(TEXT("PotionSlot")));
	ChattingWidget = Cast<UChattingWidgetBase>(GetWidgetFromName(TEXT("ChattingWidget")));
	CurrentGunText = Cast<UTextBlock>(GetWidgetFromName(TEXT("CurrentGunText")));
	CurrentGunUpgradeText = Cast<UTextBlock>(GetWidgetFromName(TEXT("CurrentGunUpgradeText")));
}

void UBattleWidgetBase::SetHPBar(float Percent)
{
	HPBar->SetPercent(Percent);
}

void UBattleWidgetBase::SetHPBarColor(FLinearColor color)
{
	HPBar->SetFillColorAndOpacity(color);
}

void UBattleWidgetBase::SetExpBar(float Percent)
{
	ExpBar->SetPercent(Percent);
}

void UBattleWidgetBase::SetMoney(int AddMoney)
{
	Money->SetText(UKismetTextLibrary::Conv_IntToText(AddMoney));
}

void UBattleWidgetBase::SetLevel(int AddLevel)
{
	Level->SetText(UKismetTextLibrary::Conv_IntToText(AddLevel));
}

void UBattleWidgetBase::SetPotionSlot(UMaterialInstance * NewMaterial)
{
	if (PotionSlot)
	{
		PotionSlot->SetBrushColor(FLinearColor(1.f, 1.f, 1.f, 1.f));
		PotionSlot->SetBrushFromMaterial(NewMaterial);
	}
}

void UBattleWidgetBase::EmptyPotionSlot()
{
	if (PotionSlot)
	{
		PotionSlot->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.3f));
		PotionSlot->SetBrushFromMaterial(nullptr);
	}
}

void UBattleWidgetBase::SetCurrentGunText(FString GunName)
{
	if (CurrentGunText)
	{
		CurrentGunText->SetText(FText::FromString(GunName));
	}
}

void UBattleWidgetBase::SetCurrentGunUpgradeText(int GunUpgrade)
{
	if (CurrentGunUpgradeText)
	{
		FString NewText = FString::Printf(TEXT("%d 강"), GunUpgrade);
		CurrentGunUpgradeText->SetText(FText::FromString(NewText));
	}
}
