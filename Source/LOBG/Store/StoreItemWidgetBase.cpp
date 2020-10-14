// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreItemWidgetBase.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"

void UStoreItemWidgetBase::NativeConstruct()
{
	ItemBorder = Cast<UBorder>(GetWidgetFromName(TEXT("ItemBorder")));
	ItemButton = Cast<UButton>(GetWidgetFromName(TEXT("ItemButton")));
	ItemText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemText")));

	ItemButton->OnClicked.AddDynamic(this, &UStoreItemWidgetBase::ClickedItemButton);
}

void UStoreItemWidgetBase::ClickedItemButton()
{
	switch (MyItemName)
	{
	case EItemName::AttackPointUp:
		UE_LOG(LogClass, Warning, TEXT("AttackPointUp"));
		break;
	case EItemName::SpeedUp:
		UE_LOG(LogClass, Warning, TEXT("SpeedUp"));
		break;
	case EItemName::FullHP:
		UE_LOG(LogClass, Warning, TEXT("FullHP"));
		break;
	default:
		break;
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
