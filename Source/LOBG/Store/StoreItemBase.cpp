// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreItemBase.h"
#include "Components/Border.h"
#include "Components/Button.h"

void UStoreItemBase::NativeConstruct()
{
	ItemBorder = Cast<UBorder>(GetWidgetFromName(TEXT("ItemColor")));
	ItemButton = Cast<UButton>(GetWidgetFromName(TEXT("ItemButton")));

	ItemButton->OnClicked.AddDynamic(this, &UStoreItemBase::ClickItemSlot);
}

void UStoreItemBase::SetItemColor(FLinearColor newColor)
{
	ItemBorder->SetBrushColor(newColor);
}

void UStoreItemBase::ClickItemSlot()
{
	
}
