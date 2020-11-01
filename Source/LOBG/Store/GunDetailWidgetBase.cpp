// Fill out your copyright notice in the Description page of Project Settings.


#include "GunDetailWidgetBase.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

void UGunDetailWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	GunImage = Cast<UBorder>(GetWidgetFromName(TEXT("GunImage")));
	GunName = Cast<UTextBlock>(GetWidgetFromName(TEXT("GunName")));
	GunDescription = Cast<UTextBlock>(GetWidgetFromName(TEXT("GunDescription")));
}

void UGunDetailWidgetBase::SetGunImage(UMaterialInstance* NewMaterial)
{
	if (GunImage)
	{
		GunImage->SetBrushFromMaterial(NewMaterial);
	}
}

void UGunDetailWidgetBase::SetGunName(FString ThisGunName)
{
	if (GunName)
	{
		GunName->SetText(FText::FromString(ThisGunName));
	}
}

void UGunDetailWidgetBase::SetGunDescription(FString ThisGunName)
{
	if (GunDescription)
	{
		GunDescription->SetText(FText::FromString(ThisGunName));
	}
}
