// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshWidgetBase.h"
#include "Components/Button.h"
#include "../Battle/BattlePC.h"

void UMeshWidgetBase::NativeConstruct()
{
	FemaleButton = Cast<UButton>(GetWidgetFromName(TEXT("FemaleButton")));
	MaleButton = Cast<UButton>(GetWidgetFromName(TEXT("MaleButton")));

	FemaleButton->OnClicked.AddDynamic(this, &UMeshWidgetBase::ClickedButton);
	MaleButton->OnClicked.AddDynamic(this, &UMeshWidgetBase::ClickedButton);
}

void UMeshWidgetBase::ClickedButton()
{
	UE_LOG(LogClass, Warning, TEXT("ClickedButton"));
	bHasMesh = true;
	ABattlePC* PC = Cast<ABattlePC>(GetOwningPlayer());
	if (PC)
	{
		PC->Server_MakePlayerInGM();
	}
}
