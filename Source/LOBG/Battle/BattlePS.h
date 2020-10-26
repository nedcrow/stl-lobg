// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "../LOBGGameInstance.h"
#include "../ChoiceMesh/MeshWidgetBase.h"
#include "BattlePS.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FPlayerData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		EMeshType PlayerMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		ETeamColor PlayerTeamColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float PlayerAttackData = 5.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float PlayerSpeedData = 300.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString PlayerName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float PlayerMoneyData;
};
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "State")
		float AttackPoint = 5.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "State")
		float PlayerSpeed = 300.f;

	float NextExp = 200;
	float TempExp = 0;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "State")
	float NewExp = 0;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "State")
	ETeamColor TeamColor = ETeamColor::None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "State")
		EMeshType PlayerMeshType = EMeshType::None;

	FPlayerData MyPlayerData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "State")
		float FireBulletAngle = 3.f;
};
