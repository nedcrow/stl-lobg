// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleHUD.h"
#include "Engine/StreamableManager.h"
#include "Engine/Canvas.h"

void ABattleHUD::DrawHUD()
{
	Super::DrawHUD();

	//�ε带 ������ �ߴٸ� �׸���
	if (CrossAsset)
	{
		DrawTextureSimple(CrossAsset, (Canvas->SizeX / 2) - (CrossAsset->GetSizeX() / 2), (Canvas->SizeY / 2) - (CrossAsset->GetSizeY() / 2));
	}
}

void ABattleHUD::BeginPlay()
{
	Super::BeginPlay();
	//�������
	T_CrossAsset.SetPath(TEXT("Texture2D'/Game/UI/Crossshair/crosshair.crosshair'"));

	//�ؽ�Ʈ �ε�
	FStreamableManager loader;
	loader.RequestAsyncLoad(T_CrossAsset, FStreamableDelegate::CreateUObject(this, &ABattleHUD::LoadHUDAsset));
}

void ABattleHUD::LoadHUDAsset()
{
	CrossAsset = Cast<UTexture2D>(T_CrossAsset.TryLoad());
}
