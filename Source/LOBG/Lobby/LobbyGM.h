// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGM.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API ALobbyGM : public AGameModeBase
{
	GENERATED_BODY()
public:

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void BeginPlay() override;

	void StartGame();
	void CountConnect();
	void MakeTeam(const FString& UserName);

	TArray<FString> TeamRedNameArray;
	TArray<FString> TeamBlueNameArray;

	void ChangeTeam(const FString& UserName);
};
