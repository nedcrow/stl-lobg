// Fill out your copyright notice in the Description page of Project Settings.


#include "TitlePC.h"
#include "TitleWidgetBase.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

void ATitlePC::BeginPlay()
{
	Super::BeginPlay();
	
	if (TitleWidgetClass && IsLocalPlayerController())
	{
		TitleWidgetObject = CreateWidget<UTitleWidgetBase>(this, TitleWidgetClass);
		TitleWidgetObject->AddToViewport();

		bShowMouseCursor = true;
		SetInputMode(FInputModeUIOnly());
	}
}

void ATitlePC::StartServer()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Step02_Lobby"), true, TEXT("listen"));
}

void ATitlePC::ConnectServer(FString ServerAddress)
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(*ServerAddress));
}
