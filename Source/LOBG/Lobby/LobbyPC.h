// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPC.generated.h"

/**
 *
 */
UCLASS()
class LOBG_API ALobbyPC : public APlayerController
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		TSubclassOf<class ULobbyWidgetBase> LobbyWidgetClass;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class ULobbyWidgetBase* LobbyWidgetObject;

	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
		void Server_SendMessage(const FText& Message);
	void Server_SendMessage_Implementation(const FText& Message);

	UFUNCTION(Client, Reliable)
		void Client_SendMessage(const FText& Message);
	void Client_SendMessage_Implementation(const FText& Message);

	UFUNCTION(Server, Reliable)
		void Server_SetTeamColor(const FString& NewName);
	void Server_SetTeamColor_Implementation(const FString& NewName);

	UFUNCTION(Client, Reliable)
		void Client_SetTeamColor(const FString& NewName);
	void Client_SetTeamColor_Implementation(const FString& NewName);
};
