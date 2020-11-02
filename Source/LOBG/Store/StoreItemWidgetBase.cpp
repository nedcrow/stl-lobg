// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreItemWidgetBase.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "../Battle/BattleCharacter.h"
#include "../Battle/BattlePS.h"
#include "../Battle/BattlePC.h"
#include "Styling/WidgetStyle.h"
#include "GunDetailWidgetBase.h"
#include "StoreWidgetBase.h"

void UStoreItemWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	ItemBorder = Cast<UBorder>(GetWidgetFromName(TEXT("ItemBorder")));
	ItemButton = Cast<UButton>(GetWidgetFromName(TEXT("ItemButton")));
	//ItemText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemText")));
	ItemMoney = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemMoney")));

	ItemButton->OnClicked.AddDynamic(this, &UStoreItemWidgetBase::ClickedItemButton);
	ItemButton->OnHovered.AddDynamic(this, &UStoreItemWidgetBase::HoveredItemButton);
}

void UStoreItemWidgetBase::ClickedItemButton()
{
	if (!bEnoughMoney) return;
	if (bIsSleep) return;
	ABattlePS* PS = Cast<ABattlePS>(GetOwningPlayerState());

	if (PS)
	{
		if (PS->PlayerMoney < MyItemMoney) return;
	}

	ABattleCharacter* PlayerPawn = Cast<ABattleCharacter>(GetOwningPlayerPawn());
	if (PlayerPawn)
	{
		switch (MyItemIndex)
		{
		case 0:
			PlayerPawn->SetBooty(-MyItemMoney, 0);
			PlayerPawn->ItemSpeed();
			break;
		case 1:
			PlayerPawn->SetBooty(-MyItemMoney, 0);
			PlayerPawn->Server_FullHP();
			PlayerPawn->SetPotionSlot();
			break;
		case 2:
			PlayerPawn->SetBooty(-MyItemMoney, 0);
			PlayerPawn->Server_SetPSFOV();
			//PlayerPawn->SetPSFOV();
			break;
		case 3:
			PlayerPawn->SetBooty(-MyItemMoney, 0);
			PlayerPawn->Server_AngleDown();
			break;
		case 4: case 5: case 6: case 7: case 8:
			PlayerPawn->SetBooty(-MyItemMoney, 0);
			PlayerPawn->ChangeGunMesh(MyItemName);
			break;
		case 9:
			PlayerPawn->SetBooty(-MyItemMoney, 0);
			PlayerPawn->Server_BulletSpeedUp();
			break;
		case 10:
			PlayerPawn->SetBooty(-MyItemMoney, 0);
			PlayerPawn->Server_MaxHPUp();
			break;
		case 11:
			PlayerPawn->SetBooty(-MyItemMoney, 0);
			PlayerPawn->GunUpgrade();
			break;
		default:
			break;
		}

	}
	
}

void UStoreItemWidgetBase::SetItemText(FString newText)
{
	//if (ItemText)
	//{
	//	ItemText->SetText(FText::FromString(newText));
	//}
}

void UStoreItemWidgetBase::SetItemBorder(UMaterialInstance* NewMaterial)
{
	if (ItemBorder)
	{
		ItemBorder->SetBrushFromMaterial(NewMaterial);
	}
	
}

void UStoreItemWidgetBase::SetItemMoney(int newMoney)
{
	if (ItemMoney)
	{
		FString MoneyText = FString::FromInt(newMoney);
		ItemMoney->SetText(FText::FromString(MoneyText));
	}
}

bool UStoreItemWidgetBase::InitSlotByMoney()
{
	ABattlePS* PS = Cast<ABattlePS>(GetOwningPlayerState());
	if (PS)
	{
		if (PS->PlayerMoney < MyItemMoney || bIsSleep)
		{
			ItemBorder->SetBrushColor(FLinearColor(1.f, 1.f, 1.f, 0.3f));
			FSlateBrush MyHorvered;
			MyHorvered.TintColor = FLinearColor(1.f, 1.f, 1.f, 0);
			ItemButton->WidgetStyle.SetHovered(MyHorvered);
			return false;
		}
		else
		{
			ItemBorder->SetBrushColor(FLinearColor(1.f, 1.f, 1.f, 1.f));
			FSlateBrush MyHorvered;
			MyHorvered.TintColor = FLinearColor(1.f, 1.f, 1.f, 0.5f);
			ItemButton->WidgetStyle.SetHovered(MyHorvered);
			return true;
		}
	}
	return false;
}

void UStoreItemWidgetBase::HoveredItemButton()
{
	ABattlePC* PC = Cast<ABattlePC>(GetOwningPlayer());
	if (PC)
	{
		PC->StoreWidgetObject->StoreGunDetail->SetGunImage(MyItemImage);
		PC->StoreWidgetObject->StoreGunDetail->SetGunName(MyItemName);
		PC->StoreWidgetObject->StoreGunDetail->SetGunDescription(MyItemDescription);
		PC->StoreWidgetObject->StoreGunDetail->SetVisibility(ESlateVisibility::Visible);
	}
}
