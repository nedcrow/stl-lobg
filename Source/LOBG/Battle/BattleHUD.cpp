// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleHUD.h"
#include "Engine/StreamableManager.h"
#include "Engine/Canvas.h"

void ABattleHUD::DrawHUD()
{
	Super::DrawHUD();

	if (CrosshairAsset)
	{
		DrawTextureSimple(CrosshairAsset, (Canvas->SizeX / 2) - (CrosshairAsset->GetSizeX() / 2), (Canvas->SizeY / 2) - (CrosshairAsset->GetSizeY() / 2));
	}
}

void ABattleHUD::BeginPlay()
{
	Super::BeginPlay();
}
