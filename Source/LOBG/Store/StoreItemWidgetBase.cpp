// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreItemWidgetBase.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "../Battle/BattleCharacter.h"
#include "../Battle/BattlePS.h"
#include "Styling/WidgetStyle.h"

void UStoreItemWidgetBase::NativeConstruct()
{
	ItemBorder = Cast<UBorder>(GetWidgetFromName(TEXT("ItemBorder")));
	ItemButton = Cast<UButton>(GetWidgetFromName(TEXT("ItemButton")));
	ItemText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemText")));
	ItemMoney = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemMoney")));

	ItemButton->OnClicked.AddDynamic(this, &UStoreItemWidgetBase::ClickedItemButton);
}

void UStoreItemWidgetBase::ClickedItemButton()
{
	if (!bEnoughMoney) return;
	ABattleCharacter* PlayerPawn = Cast<ABattleCharacter>(GetOwningPlayerPawn());
	if (PlayerPawn)
	{
		switch (MyItemIndex)
		{
		case 0:
			PlayerPawn->Server_SetBooty(-MyItemMoney, 0);
			PlayerPawn->Server_ItemSpeed();
			break;
		case 1:
			PlayerPawn->Server_SetBooty(-MyItemMoney, 0);
			PlayerPawn->Server_AngleDown();
			break;
		case 2:
			PlayerPawn->Server_SetBooty(-MyItemMoney, 0);
			PlayerPawn->Server_SetPSFOV();
			//PlayerPawn->SetPSFOV();
			break;
		case 3:
			PlayerPawn->Server_SetBooty(-MyItemMoney, 0);
			PlayerPawn->Server_ItemHP();
			break;
		case 4:
			PlayerPawn->Server_SetBooty(-MyItemMoney, 0);
			PlayerPawn->Server_ItemAttack();
			break;
		default:
			break;
		}

	}
	
}

void UStoreItemWidgetBase::SetItemText(FString newText)
{
	if (ItemText)
	{
		ItemText->SetText(FText::FromString(newText));
	}
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
		if (PS->PlayerMoney < MyItemMoney)
		{
			UE_LOG(LogClass, Warning, TEXT("Not Enough Money"));
			ItemBorder->SetBrushColor(FLinearColor(1.f, 1.f, 1.f, 0.3f));
			FSlateBrush MyHorvered;
			MyHorvered.TintColor = FLinearColor(1.f, 1.f, 1.f, 0);
			ItemButton->WidgetStyle.SetHovered(MyHorvered);
			return false;
		}
		else
		{
			UE_LOG(LogClass, Warning, TEXT("Enough Money"));
			ItemBorder->SetBrushColor(FLinearColor(1.f, 1.f, 1.f, 1.f));
			FSlateBrush MyHorvered;
			MyHorvered.TintColor = FLinearColor(1.f, 1.f, 1.f, 0.5f);
			ItemButton->WidgetStyle.SetHovered(MyHorvered);
			return true;
		}
	}
	return false;
}
