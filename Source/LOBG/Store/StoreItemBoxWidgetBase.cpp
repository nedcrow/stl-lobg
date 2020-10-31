// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreItemBoxWidgetBase.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "StoreItemWidgetBase.h"
#include "Engine/StreamableManager.h"

void UStoreItemBoxWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	GunSlotButton = Cast<UButton>(GetWidgetFromName(TEXT("GunSlotButton")));
	UpgradeSlotButton = Cast<UButton>(GetWidgetFromName(TEXT("UpgradeSlotButton")));
	ConsumeSlotButton = Cast<UButton>(GetWidgetFromName(TEXT("ConsumeSlotButton")));

	GunBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("GunBox")));
	UpgradeBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("UpgradeBox")));
	ConsumeBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("ConsumeBox")));

	GunSlotButton->OnClicked.AddDynamic(this, &UStoreItemBoxWidgetBase::ClickGunButton);
	UpgradeSlotButton->OnClicked.AddDynamic(this, &UStoreItemBoxWidgetBase::ClickUpgradeButton);
	ConsumeSlotButton->OnClicked.AddDynamic(this, &UStoreItemBoxWidgetBase::ClickConsumeButton);

	InitAllSlot();
	CollapsedAllBox();
}

void UStoreItemBoxWidgetBase::ClickGunButton()
{
	CollapsedAllBox();
	SetVisiblityItemSlot(GunBox);
}

void UStoreItemBoxWidgetBase::ClickUpgradeButton()
{
	CollapsedAllBox();
	SetVisiblityItemSlot(UpgradeBox);
}

void UStoreItemBoxWidgetBase::ClickConsumeButton()
{
	CollapsedAllBox();
	SetVisiblityItemSlot(ConsumeBox);
}

void UStoreItemBoxWidgetBase::InitSlot(UScrollBox* NewScrollBox, EItemType MyItemType, int AllSlotCount)
{
	TArray<FItemDataTableStruct*> ItemArray;

	for (int TableIndex = 0; TableIndex < AllSlotCount; ++TableIndex)
	{
		if (ItemDataTable)
		{
			FItemDataTableStruct* ItemData = ItemDataTable->FindRow<FItemDataTableStruct>(FName(*(FString::FromInt(TableIndex))), TEXT(""));
			
			if ((*ItemData).ItemType == MyItemType)
			{
				ItemArray.Add(ItemData);
			}
		}
	}

	for (int i = 0; i < NewScrollBox->GetChildrenCount(); ++i)
	{
		UStoreItemWidgetBase* ItemSlot = Cast< UStoreItemWidgetBase>(NewScrollBox->GetChildAt(i));
		if (ItemSlot)
		{
			ItemSlot->SetItemText(ItemArray[i]->ItemDescription);
			ItemSlot->SetItemMoney(ItemArray[i]->ItemPrice);

			FStreamableManager loader;
			ItemSlot->SetItemBorder(loader.LoadSynchronous<UMaterialInstance>(ItemArray[i]->ItemImage));

			ItemSlot->MyItemName = ItemArray[i]->ItemName;
			ItemSlot->MyItemIndex = ItemArray[i]->ItemIndex;
			ItemSlot->MyItemMoney = ItemArray[i]->ItemPrice;
			ItemSlot->InitSlotByMoney();
		}
	}
}

void UStoreItemBoxWidgetBase::InitAllSlot()
{
	int AllBoxCount = GunBox->GetChildrenCount() + UpgradeBox->GetChildrenCount() + ConsumeBox->GetChildrenCount();

	InitSlot(GunBox, EItemType::Gun, AllBoxCount);
	InitSlot(UpgradeBox, EItemType::Upgrade, AllBoxCount);
	InitSlot(ConsumeBox, EItemType::Consume, AllBoxCount);
}

void UStoreItemBoxWidgetBase::CollapsedAllBox()
{
	GunBox->SetVisibility(ESlateVisibility::Collapsed);
	UpgradeBox->SetVisibility(ESlateVisibility::Collapsed);
	ConsumeBox->SetVisibility(ESlateVisibility::Collapsed);
}

void UStoreItemBoxWidgetBase::CheckSlotActive(UScrollBox* NewScrollBox)
{
	for (int i = 0; i < NewScrollBox->GetChildrenCount(); ++i)
	{
		UStoreItemWidgetBase* ItemSlot = Cast< UStoreItemWidgetBase>(NewScrollBox->GetChildAt(i));
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

void UStoreItemBoxWidgetBase::CheckAllSlotActive()
{
	CheckSlotActive(GunBox);
	CheckSlotActive(UpgradeBox);
	CheckSlotActive(ConsumeBox);
}

void UStoreItemBoxWidgetBase::SetVisiblityItemSlot(UScrollBox* NewScrollBox)
{
	NewScrollBox->SetVisibility(ESlateVisibility::Visible);
}
