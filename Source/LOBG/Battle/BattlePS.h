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
		float Money;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "State")
		float Exp;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
