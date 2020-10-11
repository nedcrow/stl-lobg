// Fill out your copyright notice in the Description page of Project Settings.


#include "MouseButtonSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "TeamSlot.h"

void UMouseButtonSlot::NativeConstruct()
{
	UserName = Cast<UTextBlock>(GetWidgetFromName(TEXT("TeamText")));
	SlotButton = Cast<UButton>(GetWidgetFromName(TEXT("MouseSlotButton")));
}

void UMouseButtonSlot::SetName(FString NewName)
{
	UserName->SetText(FText::FromString(NewName));
}