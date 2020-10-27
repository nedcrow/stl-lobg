// Fill out your copyright notice in the Description page of Project Settings.


#include "MVPSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UMVPSlot::NativeConstruct() {
	Super::NativeConstruct();
	MVPIcon = Cast<UImage>(GetWidgetFromName(TEXT("MVPIcon")));
	UserIDText = Cast<UTextBlock>(GetWidgetFromName(TEXT("UserIDText")));
	UserLVText = Cast<UTextBlock>(GetWidgetFromName(TEXT("UserLVText")));
}

