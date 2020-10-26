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

	if (ItemTextArray.Num() == ItemInstanceArray.Num() && ItemBox->GetChildrenCount() == ItemTextArray.Num())
	{
		//for (int i = 0; i < ItemTextArray.Num(); ++i)
		//{
		//}
	}
	if (ItemDataTableTest)
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
			//ItemData = GetItemData(i);
			//ItemData = GetItemDataTest(i);
			if (ItemDataTableTest)
			{
				FItemDataTableTestStruct* ItemDataTest = ItemDataTableTest->FindRow<FItemDataTableTestStruct>(FName(*(FString::FormatAsNumber(i))), TEXT(""));

				ItemSlot->SetVisibility(ESlateVisibility::Collapsed);
				if (ItemDataTest)
				{
					ItemSlot->SetItemText((*ItemDataTest).ItemDescription);
					ItemSlot->SetItemMoney((*ItemDataTest).ItemPrice);

					FStreamableManager loader;
					ItemSlot->SetItemBorder(loader.LoadSynchronous<UMaterialInstance>((*ItemDataTest).ItemImage));

					//ItemSlot->MyItemName = (*ItemDataTest).ItemName;
					ItemSlot->MyItemMoney = (*ItemDataTest).ItemPrice;
				}
				
			}

			//ItemSlot->SetVisibility(ESlateVisibility::Collapsed);
			//
			//ItemSlot->SetItemText(ItemData.ItemDescription);
			//ItemSlot->SetItemMoney(ItemData.ItemPrice);
			//
			//FStreamableManager loader;
			//ItemSlot->SetItemBorder(loader.LoadSynchronous<UMaterialInstance>//(ItemData.ItemImage));
			//
			////ItemSlot->MyItemName = ItemData.ItemName;
			//ItemSlot->MyItemMoney = ItemData.ItemPrice;



			//ItemSlot->SetItemText(ItemTextArray[i]);
			//ItemSlot->SetItemMoney(ItemMoneyArray[i]);
			//ItemSlot->SetItemBorder(ItemInstanceArray[i]);
			//ItemSlot->MyItemName = ItemNameArray[i];
			//ItemSlot->MyItemMoney = ItemMoneyArray[i];

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
