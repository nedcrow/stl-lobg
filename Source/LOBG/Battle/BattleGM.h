// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "../LOBGGameInstance.h"
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
	virtual void PostLogin(APlayerController* NewPlayer) override;

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

	//Towerclass �ޱ�
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Object")
		TSubclassOf<class ATempTower> TempTowerClass;

	//�ʻ��� Ÿ���� �پ��� -ī��Ʈ�ϰ� Ÿ���� 0�̸� GoLobby����
	void CountTower();

	int TowerCount;

	//�κ�� ���ư���
	void GoLobby();

	void TestUIColor();
};
