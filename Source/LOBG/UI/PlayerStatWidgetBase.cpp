// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatWidgetBase.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"

void UPlayerStatWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	AttackStat = Cast<UTextBlock>(GetWidgetFromName(TEXT("AttackStat")));
	SpeedStat = Cast<UTextBlock>(GetWidgetFromName(TEXT("SpeedStat")));
	MoneyStat = Cast<UTextBlock>(GetWidgetFromName(TEXT("MoneyStat")));
}

void UPlayerStatWidgetBase::SetStatText(int AttackPoint, int SpeedPoint, int MoneyPoint)
{
	AttackStat->SetText(UKismetTextLibrary::Conv_IntToText(AttackPoint));
	SpeedStat->SetText(UKismetTextLibrary::Conv_IntToText(SpeedPoint));
	MoneyStat->SetText(UKismetTextLibrary::Conv_IntToText(MoneyPoint));
}
