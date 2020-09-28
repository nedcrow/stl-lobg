// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BattlePS.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API ABattlePS : public APlayerState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing="OnRep_Level", Category = "State")
	int PlayerLevel;

	UFUNCTION()
		void OnRep_Level();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing="OnRep_Money", Category = "State")
		int PlayerMoney = 0;

	UFUNCTION()
		void OnRep_Money();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing="OnRep_Exp", Category = "State")
		float PlayerExp = 0;

	UFUNCTION()
	void OnRep_Exp();

	float NextExp = 200;

	void SetExp(float AddExp);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = "OnRep_Exp", Category = "State")
	float NewExp = 0;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
