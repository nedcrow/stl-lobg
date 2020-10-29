// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "../LOBGGameInstance.h"
#include "../ChoiceMesh/MeshWidgetBase.h"
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
		TArray<class AAIManager*> AIManagers;	

	class AAIManager* FindAIM(class AAIController* AIPawnOwner);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		int SpawnAINumber = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		float WaveRepeatSeconds = 60.f;

	FTimerHandle RepeatWaveHandle;

	UFUNCTION()
		void RepeatMinionsWave();

	//������ ��û
	void CallReSpawn(class ABattleCharacter* Pawn);

	//������ ��ų Player�� �����Ϳ��� ����
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Player")
		TSubclassOf<class ABattleCharacter> PlayerClass;

	//Towerclass �ޱ�
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Object")
		TSubclassOf<class ATempTower> TempTowerClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Object")
		TSubclassOf<class AFairyPawn> FairyTowerClass;

	void FindTowers();

	//�ʻ��� Ÿ���� �پ��� -ī��Ʈ�ϰ� Ÿ���� 0�̸� GoLobby����
	void CountTower(ETeamColor DestroyTowerColor);

	int RedTowerCount = 1;
	int BlueTowerCount = 1;

	//�κ�� ���ư���
	void GoLobby();

	void FindPlayerStart();

	TArray<AActor*> OutputPlayerStart;

	TArray<FString> TeamRedUsers;
	TArray<FString> TeamBlueUsers;
	TArray<bool> CheckControllerHasName;

	void SetPSTeamColor();

	void PlayerSpawn();

	void PlayerSpawn_Test(class ABattlePC* Controller, EMeshType MyMeshType);

	void CheckAllControllerHasName();

	bool TestMapVersonSpawn;

	void CreatePlayerMeshWidget();

	void SetGameStartTimeWidget(int GameTime);

	void StartAIMinion();

	void CallOpenResultTab(ETeamColor WinColor);
};
