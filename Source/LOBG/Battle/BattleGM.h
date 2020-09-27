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
	//리스폰 요청
	void CallReSpawn(class ABattleCharacter* Pawn);

	//리스폰 시킬 Player를 에디터에서 설정
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Player")
		TSubclassOf<class ABattleCharacter> PlayerClass;

};
