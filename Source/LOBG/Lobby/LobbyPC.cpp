// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPC.h"
#include "UI/LobbyWidgetBase.h"
#include "UI/ChattingWidgetBase.h"

void ALobbyPC::BeginPlay() {
	Super::BeginPlay();
	if (LobbyWidgetClass && IsLocalPlayerController()) {
		LobbyWidgetObject = CreateWidget<ULobbyWidgetBase>(this, LobbyWidgetClass);
		LobbyWidgetObject->AddToViewport();

		bShowMouseCursor = true;
		SetInputMode(FInputModeGameAndUI());
	}
}

void ALobbyPC::Server_SendMessage_Implementation(const FText& Message)
{
	if (LobbyWidgetObject) {
		LobbyWidgetObject->ChattingWidget->AddMessage(Message);
	}
}

void ALobbyPC::Client_SendMessage_Implementation(const FText& Message)
{
	for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; Iter++) {
		ALobbyPC* PC = Cast<ALobbyPC>(*Iter);
		if (PC) {
			PC->Server_SendMessage(Message);
		}
	}
}
