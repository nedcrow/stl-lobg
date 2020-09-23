// Fill out your copyright notice in the Description page of Project Settings.


#include "TitlePC.h"
#include "TitleWidgetBase.h"
#include "Blueprint/UserWidget.h"

void ATitlePC::BeginPlay()
{
	Super::BeginPlay();
	
	if (TitleWidgetClass)
	{
		TitleWidgetObject = CreateWidget<UTitleWidgetBase>(this, TitleWidgetClass);
		TitleWidgetObject->AddToViewport();

		bShowMouseCursor = true;
		SetInputMode(FInputModeUIOnly());
	}
}
