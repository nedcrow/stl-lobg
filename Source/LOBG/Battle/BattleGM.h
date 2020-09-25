// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleGM.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API ABattleGM : public AGameModeBase
{
	GENERATED_BODY()

public:
	//미완성임 건들지 말것
	void CallReSpawn(class ABattleCharacter* Pawn);

	UFUNCTION(NetMulticast, Reliable)
		void SetReSpawnPawnSetting(class ABattleCharacter* Pawn, FVector location);
		void SetReSpawnPawnSetting_Implementation(class ABattleCharacter* Pawn, FVector location);
};
