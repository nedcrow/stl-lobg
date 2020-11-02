// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreItemBoxWidgetBase.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "StoreItemWidgetBase.h"
#include "Engine/StreamableManager.h"
#include "../Battle/BattleCharacter.h"
#include "../Battle/BattlePS.h"

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
			//ItemSlot->SetItemText(ItemArray[i]->ItemDescription);
			ItemSlot->SetItemMoney(ItemArray[i]->ItemPrice);

			FStreamableManager loader;
			UMaterialInstance* MI = loader.LoadSynchronous<UMaterialInstance>(ItemArray[i]->ItemImage);
			ItemSlot->SetItemBorder(MI);
			ItemSlot->MyItemImage = MI;

			ItemSlot->MyItemName = ItemArray[i]->ItemName;
			ItemSlot->MyItemIndex = ItemArray[i]->ItemIndex;
			ItemSlot->MyItemMoney = ItemArray[i]->ItemPrice;
			ItemSlot->MyItemDescription = ItemArray[i]->ItemDescription;
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

FItemDataTableStruct UStoreItemBoxWidgetBase::GetItemData(int Index) const
{
	return *ItemDataTable->FindRow<FItemDataTableStruct>(*FString::FromInt(Index), TEXT("ItemIndex"));
}

void UStoreItemBoxWidgetBase::SetUpgradeGunUpdate(FString CurrentGunName, int DataIndex)
{
	for (int i = 0; i < UpgradeBox->GetChildrenCount(); ++i)
	{
		UStoreItemWidgetBase* ItemSlot = Cast< UStoreItemWidgetBase>(UpgradeBox->GetChildAt(i));
		if (ItemSlot)
		{
			//ItemSlot->SetItemText(ItemArray[i]->ItemDescription);
			
			if (ItemSlot->MyItemName == FString("GunUpgrade"))
			{
				ABattleCharacter* PlayerPawn = GetOwningPlayerPawn<ABattleCharacter>();
				if (PlayerPawn)
				{
					//FString CurrentPlayerGunName = PlayerPawn->CurrentGunName;
					if (CurrentGunName != FString(""))
					{
						ABattlePS* PS = PlayerPawn->GetPlayerState<ABattlePS>();
						if (PS)
						{
							TArray<FGunUpgradeData> TempGunData = PS->GunDataArray;
							for (int GunIndex = 0; GunIndex < TempGunData.Num(); ++GunIndex)
							{
								if (TempGunData[GunIndex].GunName == CurrentGunName)
								{
									switch (TempGunData[GunIndex].UpgradeCount)
									{
									case 0:
										ItemSlot->SetItemMoney(GetItemData(DataIndex).UpgradePriceOne);
										ItemSlot->MyItemMoney = GetItemData(DataIndex).UpgradePriceOne;
										break;
									case 1:
										ItemSlot->SetItemMoney(GetItemData(DataIndex).UpgradePriceTwo);
										ItemSlot->MyItemMoney = GetItemData(DataIndex).UpgradePriceTwo;
										break;
									case 2:
										ItemSlot->SetItemMoney(GetItemData(DataIndex).UpgradePriceThree);
										ItemSlot->MyItemMoney = GetItemData(DataIndex).UpgradePriceThree;
										break;
									default:
										break;
									}
									break;
								}
							}
						}
					}
				}
				//if (!ItemSlot->bIsSleep)
				//{
					ItemSlot->InitSlotByMoney();
				//}
			}
		}
	}
}

void UStoreItemBoxWidgetBase::CheckSleepSlot()
{
	for (int i = 0; i < UpgradeBox->GetChildrenCount(); ++i)
	{
		UStoreItemWidgetBase* ItemSlot = Cast<UStoreItemWidgetBase>(UpgradeBox->GetChildAt(i));
		if (ItemSlot)
		{
			if (ItemSlot->MyItemName == FString("GunUpgrade"))
			{
				ItemSlot->bIsSleep = true;
				ItemSlot->ItemBorder->SetBrushColor(FLinearColor(1.f, 1.f, 1.f, 0.3f));
				FSlateBrush MyHorvered;
				MyHorvered.TintColor = FLinearColor(1.f, 1.f, 1.f, 0);
				ItemSlot->ItemButton->WidgetStyle.SetHovered(MyHorvered);
				break;
			}
		}
	}
}

void UStoreItemBoxWidgetBase::WakeUpSlot()
{
	for (int i = 0; i < UpgradeBox->GetChildrenCount(); ++i)
	{
		UStoreItemWidgetBase* ItemSlot = Cast< UStoreItemWidgetBase>(UpgradeBox->GetChildAt(i));
		if (ItemSlot)
		{
			if (ItemSlot->MyItemName == FString("GunUpgrade"))
			{
				if (ItemSlot->bIsSleep)
				{
					ItemSlot->bIsSleep = false;
					ItemSlot->ItemBorder->SetBrushColor(FLinearColor(1.f, 1.f, 1.f, 1.f));
					FSlateBrush MyHorvered;
					MyHorvered.TintColor = FLinearColor(1.f, 1.f, 1.f, 0.5f);
					ItemSlot->ItemButton->WidgetStyle.SetHovered(MyHorvered);
					break;
				}
			}
		}
	}
}
