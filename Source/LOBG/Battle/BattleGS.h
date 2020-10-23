// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BattlePS.h"
#include "BattleGS.generated.h"



/**
 * 
 */
UCLASS()
class LOBG_API ABattleGS : public AGameStateBase
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = "OnRep_GameStartTime")
		int GameStartTime = 0;

	UFUNCTION()
		void OnRep_GameStartTime();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
		TArray<FPlayerData> RedTabDataArray;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
		TArray<FPlayerData> BlueTabDataArray;
};
