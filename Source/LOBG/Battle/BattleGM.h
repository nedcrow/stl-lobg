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
	virtual void BeginPlay() override;

	// AIManager
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		TSubclassOf<class AAIManager> AIManagerClass;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		class AAIManager* AIManager;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		int SpawnAINumber = 0;



	//������ ��û
	void CallReSpawn(class ABattleCharacter* Pawn);

	//������ ��ų Player�� �����Ϳ��� ����
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Player")
		TSubclassOf<class ABattleCharacter> PlayerClass;

	void LevelUp();


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Object")
		TSubclassOf<class ATempTower> TempTowerClass;

	void CountTower();

	int TowerCount;

	void GoLobby();
};
