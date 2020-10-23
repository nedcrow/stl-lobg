// Fill out your copyright notice in the Description page of Project Settings.


#include "TabSlotWidgetBase.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PlayerStatWidgetBase.h"

void UTabSlotWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	PlayerMeshImage = Cast<UImage>(GetWidgetFromName(TEXT("PlayerMeshImage")));
	PlayerName = Cast<UTextBlock>(GetWidgetFromName(TEXT("PlayerName")));
	PlayerStatUI = Cast<UPlayerStatWidgetBase>(GetWidgetFromName(TEXT("PlayerStatUI")));
}

void UTabSlotWidgetBase::SetPlayerMesh(UMaterialInstance * PlayerMesh)
{
	PlayerMeshImage->SetBrushFromMaterial(PlayerMesh);
}

void UTabSlotWidgetBase::SetPlayerName(FString UserName)
{
	PlayerName->SetText(FText::FromString(UserName));
}
