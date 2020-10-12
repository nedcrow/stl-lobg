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
		TArray<class AAIManager*> AIManagers;	

	class AAIManager* FindAIM(class AAIController* AIPawnOwner);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		int SpawnAINumber = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		float WaveRepeatSeconds = 60.f;

	FTimerHandle RepeatWaveHandle;

	UFUNCTION()
		void RepeatMinionsWave();

	//리스폰 요청
	void CallReSpawn(class ABattleCharacter* Pawn);

	//리스폰 시킬 Player를 에디터에서 설정
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Player")
		TSubclassOf<class ABattleCharacter> PlayerClass;

	//Towerclass 받기
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Object")
		TSubclassOf<class ATempTower> TempTowerClass;

	//맵상의 타워가 줄어들면 -카운트하고 타워가 0이면 GoLobby실행
	void CountTower();

	int TowerCount;

	//로비로 돌아가기
	void GoLobby();

	void FindPlayerStart();

	TArray<AActor*> OutputPlayerStart;

	TArray<FString> TeamRedUsers;
	TArray<FString> TeamBlueUsers;
	TArray<bool> CheckControllerHasName;

	void SetPSTeamColor();

	void PlayerSpawn();

	void CheckAllControllerHasName();

	bool TestMapVersonSpawn;
};
