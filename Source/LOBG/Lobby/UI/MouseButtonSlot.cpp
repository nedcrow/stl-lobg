// Fill out your copyright notice in the Description page of Project Settings.


#include "MouseButtonSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UMouseButtonSlot::NativeConstruct()
{
	UserName = Cast<UTextBlock>(GetWidgetFromName(TEXT("TeamText")));
	SlotButton = Cast<UButton>(GetWidgetFromName(TEXT("TeamText")));

	SlotButton->OnClicked.AddDynamic(this, &UMouseButtonSlot::ChangeTeam);
}

void UMouseButtonSlot::ChangeTeam()
{
}
