// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleHUD.h"
#include "Engine/StreamableManager.h"
#include "Engine/Canvas.h"

void ABattleHUD::DrawHUD()
{
	Super::DrawHUD();

	//로드를 무사히 했다면 그리기
	if (CrossAsset)
	{
		DrawTextureSimple(CrossAsset, (Canvas->SizeX / 2) - (CrossAsset->GetSizeX() / 2), (Canvas->SizeY / 2) - (CrossAsset->GetSizeY() / 2));
	}
}

void ABattleHUD::BeginPlay()
{
	Super::BeginPlay();
	//경로지정
	T_CrossAsset.SetPath(TEXT("Texture2D'/Game/UI/Crossshair/crosshair.crosshair'"));

	//텍스트 로드
	FStreamableManager loader;
	loader.RequestAsyncLoad(T_CrossAsset, FStreamableDelegate::CreateUObject(this, &ABattleHUD::LoadHUDAsset));
}

void ABattleHUD::LoadHUDAsset()
{
	CrossAsset = Cast<UTexture2D>(T_CrossAsset.TryLoad());
}
