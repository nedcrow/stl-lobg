// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPC.h"
#include "UI/LobbyWidgetBase.h"
#include "UI/ChattingWidgetBase.h"
#include "../LOBGGameInstance.h"

void ALobbyPC::BeginPlay() {
	Super::BeginPlay();
	if (LobbyWidgetClass && IsLocalPlayerController()) {
		LobbyWidgetObject = CreateWidget<ULobbyWidgetBase>(this, LobbyWidgetClass);
		LobbyWidgetObject->AddToViewport();

		bShowMouseCursor = true;
		SetInputMode(FInputModeGameAndUI());

		ULOBGGameInstance* GI = GetGameInstance<ULOBGGameInstance>();
		if (GI)
		{
			const FString UserName = GI->GetUserID();
			Server_SetTeamColor(UserName);
		}
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

void ALobbyPC::Server_SetTeamColor_Implementation(const FString& NewName)
{
	for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; Iter++)
	{
		ALobbyPC* PC = Cast<ALobbyPC>(*Iter);
		if (PC)
		{
			PC->Client_SetTeamColor(NewName);
		}
	}
}

void ALobbyPC::Client_SetTeamColor_Implementation(const FString& NewName)
{
	if (LobbyWidgetObject)
	{
		LobbyWidgetObject->SplitTeam(NewName);
	}
}
