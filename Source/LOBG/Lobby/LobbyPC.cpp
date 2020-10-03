// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPC.h"
#include "UI/LobbyWidgetBase.h"
#include "UI/ChattingWidgetBase.h"
#include "../LOBGGameInstance.h"
#include "LobbyGM.h"
#include "Kismet/GameplayStatics.h"

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
			MyUserID = GI->GetUserID();
		}
		Server_BeginPC(MyUserID);
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

FString ALobbyPC::GetGIUserID()
{
	ULOBGGameInstance* GI = GetGameInstance<ULOBGGameInstance>();
	if (GI)
	{
		return GI->GetUserID();
	}
	return FString();
}

void ALobbyPC::Client_SplitTeam_Implementation(const TArray<FString>& NewArray)
{
	if (IsLocalPlayerController() && LobbyWidgetObject)
	{
		LobbyWidgetObject->SplitTeam(NewArray);
		
	}
}

void ALobbyPC::Server_BeginPC_Implementation(const FString& UserName)
{
	ALobbyGM* GM = Cast<ALobbyGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		GM->MakeTeam(this, UserName);
	}
}
