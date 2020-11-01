// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreWidgetBase.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "StoreItemWidgetBase.h"
#include "StoreItemBoxWidgetBase.h"
#include "GunDetailWidgetBase.h"
#include "Components/ScrollBox.h"
#include "Engine/StreamableManager.h"

void UStoreWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	ItemBox = Cast<UStoreItemBoxWidgetBase>(GetWidgetFromName(TEXT("StoreItemBox")));
	StoreBorder = Cast<UBorder>(GetWidgetFromName(TEXT("StoreBorder")));
	StoreGunDetail = Cast<UGunDetailWidgetBase>(GetWidgetFromName(TEXT("StoreGunDetail")));

	if (StoreGunDetail)
	{
		StoreGunDetail->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UStoreWidgetBase::SetVisiBilitySlot(ESlateVisibility NewValue)
{
	SetVisibility(NewValue);

	if (StoreGunDetail)
	{
		StoreGunDetail->SetVisibility(ESlateVisibility::Collapsed);
	}
}

FItemDataTableStruct UStoreWidgetBase::GetItemData(int Index) const
{
	return *ItemDataTable->FindRow<FItemDataTableStruct>(*FString::FromInt(Index), TEXT("ItemIndex"));
}
