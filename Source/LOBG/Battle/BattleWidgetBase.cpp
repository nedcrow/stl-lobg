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
	HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HPBar")));
	ExpBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ExpBar")));
}

void UBattleWidgetBase::SetHPBar(float Percent)
{
	HPBar->SetPercent(Percent);
}

void UBattleWidgetBase::SetExpBar(float Percent)
{
	UE_LOG(LogClass, Warning, TEXT("InputWidgetExp"));
	ExpBar->SetPercent(Percent);
}

void UBattleWidgetBase::SetMoney(int AddMoney)
{
	UE_LOG(LogClass, Warning, TEXT("InputWidgetMoney"));
	Money->SetText(UKismetTextLibrary::Conv_IntToText(AddMoney));
}

void UBattleWidgetBase::SetLevel(int AddLevel)
{
	Level->SetText(UKismetTextLibrary::Conv_IntToText(AddLevel));
}
