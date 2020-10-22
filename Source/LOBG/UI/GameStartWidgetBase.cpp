// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStartWidgetBase.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"

void UGameStartWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	GameStartTime = Cast<UTextBlock>(GetWidgetFromName(TEXT("GameStartTime")));
}

void UGameStartWidgetBase::SetGameStartTimeText(int LeftTime)
{
	GameStartTime->SetText(UKismetTextLibrary::Conv_IntToText(LeftTime));
}
