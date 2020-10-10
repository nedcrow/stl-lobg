// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleWidgetBase.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetTextLibrary.h"

void UBattleWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	Money = Cast<UTextBlock>(GetWidgetFromName(TEXT("MoneyText")));
	Level = Cast<UTextBlock>(GetWidgetFromName(TEXT("LevelText")));
	CountPlayer = Cast<UTextBlock>(GetWidgetFromName(TEXT("CountPlayerText")));
	HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HPBar")));
	ExpBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ExpBar")));
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
