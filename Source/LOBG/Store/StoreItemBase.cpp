// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreItemBase.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UStoreItemBase::NativeConstruct()
{
	AttackPointUpText = Cast<UTextBlock>(GetWidgetFromName(TEXT("AttackPointUpText")));
	AttackPointUpButton = Cast<UButton>(GetWidgetFromName(TEXT("AttackPointUpButton")));

	SpeedUpText = Cast<UTextBlock>(GetWidgetFromName(TEXT("SpeedUpText")));
	SpeedUpButton = Cast<UButton>(GetWidgetFromName(TEXT("SpeedUpButton")));

	AttackPointUpButton->OnClicked.AddDynamic(this, &UStoreItemBase::ClickItemSlot);
	SpeedUpButton->OnClicked.AddDynamic(this, &UStoreItemBase::ClickItemSlot);
}

void UStoreItemBase::SetItemColor(FLinearColor newColor)
{
	//ItemBorder->SetBrushColor(newColor);
}

void UStoreItemBase::ClickItemSlot()
{
	
}