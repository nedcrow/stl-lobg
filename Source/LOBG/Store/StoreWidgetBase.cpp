// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreWidgetBase.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "StoreItemBase.h"

void UStoreWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	//RedItem = Cast<UBorder>(GetWidgetFromName(TEXT("RedItem")));
	//BlueItem = Cast<UBorder>(GetWidgetFromName(TEXT("BlueItem")));
	//GreenItem = Cast<UBorder>(GetWidgetFromName(TEXT("GreenItem")));
	//YellowItem = Cast<UBorder>(GetWidgetFromName(TEXT("YellowItem")));
	//
	//RedButton = Cast<UButton>(GetWidgetFromName(TEXT("RedButton")));
	//BlueButton = Cast<UButton>(GetWidgetFromName(TEXT("BlueButton")));
	//GreenButton = Cast<UButton>(GetWidgetFromName(TEXT("GreenButton")));
	//YellowButton = Cast<UButton>(GetWidgetFromName(TEXT("YellowButton")));
	ItemBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("ItemBox")));
	
	InitBox();
}

void UStoreWidgetBase::InitBox()
{
	
	for (int i = 0; i < ItemBox->GetChildrenCount(); ++i)
	{
		UStoreItemBase* ItemSlot = Cast<UStoreItemBase>(ItemBox->GetChildAt(i));
		if (ItemSlot)
		{
			switch (i)
			{
			case 0:
				ItemSlot->SetItemColor(FLinearColor::Red);
				break;
			case 1:
				ItemSlot->SetItemColor(FLinearColor::Blue);
				break;
			case 2:
				ItemSlot->SetItemColor(FLinearColor::Green);
				break;
			case 3:
				ItemSlot->SetItemColor(FLinearColor::Yellow);
				break;
			}
		}
	}
}
