// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LobbyGS.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API ALobbyGS : public AGameStateBase
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = "OnRepConnectCount", Category = "Network") // 이걸 BP랑 활용하는 것이 더 효율적
		int ConnectCount = 0;

	UFUNCTION()
		void OnRepConnectCount();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
