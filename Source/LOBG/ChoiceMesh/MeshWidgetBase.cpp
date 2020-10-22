// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshWidgetBase.h"
#include "Components/Button.h"
#include "../Battle/BattlePC.h"

void UMeshWidgetBase::NativeConstruct()
{
	FemaleButton = Cast<UButton>(GetWidgetFromName(TEXT("FemaleButton")));
	MaleButton = Cast<UButton>(GetWidgetFromName(TEXT("MaleButton")));

	FemaleButton->OnClicked.AddDynamic(this, &UMeshWidgetBase::ClickedFemaleButton);
	MaleButton->OnClicked.AddDynamic(this, &UMeshWidgetBase::ClickedMaleButton);
}

void UMeshWidgetBase::ClickedFemaleButton()
{
	bHasMesh = true;
	CurrentMeshType = EMeshType::Female;
	ABattlePC* PC = Cast<ABattlePC>(GetOwningPlayer());
	if (PC)
	{
		PC->Server_MakePlayerInGM(CurrentMeshType);
	}
}

void UMeshWidgetBase::ClickedMaleButton()
{
	bHasMesh = true;
	CurrentMeshType = EMeshType::Male;
	ABattlePC* PC = Cast<ABattlePC>(GetOwningPlayer());
	if (PC)
	{
		PC->Server_MakePlayerInGM(CurrentMeshType);
	}
}
