// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreWidgetBase.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "StoreItemWidgetBase.h"
#include "Components/ScrollBox.h"
#include "Engine/StreamableManager.h"

void UStoreWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	ItemBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("ItemBox")));
	StoreBorder = Cast<UBorder>(GetWidgetFromName(TEXT("StoreBorder")));

	if (ItemDataTable)
	{
		InitItemArray();
	}
			
}

void UStoreWidgetBase::InitItemArray()
{
	for (int i = 0; i < ItemBox->GetChildrenCount(); ++i)
	{
		UStoreItemWidgetBase* ItemSlot = Cast< UStoreItemWidgetBase>(ItemBox->GetChildAt(i));
		if (ItemSlot)
		{
			if (ItemDataTable)
			{
				FItemDataTableStruct* ItemData = ItemDataTable->FindRow<FItemDataTableStruct>(FName(*(FString::FormatAsNumber(i))), TEXT(""));

				ItemSlot->SetVisibility(ESlateVisibility::Collapsed);
				if (ItemData)
				{
					ItemSlot->SetItemText((*ItemData).ItemDescription);
					ItemSlot->SetItemMoney((*ItemData).ItemPrice);

					FStreamableManager loader;
					ItemSlot->SetItemBorder(loader.LoadSynchronous<UMaterialInstance>((*ItemData).ItemImage));

					ItemSlot->MyItemName = (*ItemData).ItemName;
					ItemSlot->MyItemIndex = (*ItemData).ItemIndex;
					ItemSlot->MyItemMoney = (*ItemData).ItemPrice;
				}
				
			}

			ItemSlot->InitSlotByMoney();
		}
	}
}

void UStoreWidgetBase::SetVisiBilitySlot(ESlateVisibility NewValue)
{
	SetVisibility(NewValue);

	for (int i = 0; i < ItemBox->GetChildrenCount(); ++i)
	{
		UStoreItemWidgetBase* ItemSlot = Cast< UStoreItemWidgetBase>(ItemBox->GetChildAt(i));
		if (ItemSlot)
		{
			ItemSlot->SetVisibility(NewValue);
		}
	}
}

void UStoreWidgetBase::CheckSlotActive()
{
	for (int i = 0; i < ItemBox->GetChildrenCount(); ++i)
	{
		UStoreItemWidgetBase* ItemSlot = Cast< UStoreItemWidgetBase>(ItemBox->GetChildAt(i));
		if (ItemSlot)
		{
			if (ItemSlot->InitSlotByMoney())
			{
				ItemSlot->bEnoughMoney = true;
			}
			else
			{
				ItemSlot->bEnoughMoney = false;
			}
		}
	}
}

FItemDataTableStruct UStoreWidgetBase::GetItemData(int Index) const
{
	return *ItemDataTable->FindRow<FItemDataTableStruct>(*FString::FromInt(Index), TEXT("ItemIndex"));
}
