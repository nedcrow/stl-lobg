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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "State")
	int Level;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "State")
		int Money;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing="OnRep_Exp", Category = "State")
		float Exp = 0;

	UFUNCTION()
	void OnRep_Exp();

	float NextExp = 200;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
