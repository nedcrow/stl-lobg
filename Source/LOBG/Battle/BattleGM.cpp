// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGM.h"
#include "BattleCharacter.h"
#include "../ReSpawn/ReSpawn.h"
#include "Kismet/GameplayStatics.h"
#include "BattlePC.h"

void ABattleGM::CallReSpawn(ABattleCharacter* Pawn)
{
	if (Pawn)
	{
		UE_LOG(LogClass, Warning, TEXT("InCallReSpawn"));
		if (Pawn->CurrentState != EBattleCharacterState::Dead) return;

		AReSpawn* ReSpawnObj = Cast<AReSpawn>(UGameplayStatics::GetActorOfClass(GetWorld(), AReSpawn::StaticClass()));
		FVector SpawnLocation = ReSpawnObj->ReSpawnArea->GetComponentLocation();
		//Pawn->SetActorLocation(SpawnLocation);
		//Pawn->CurrentHP = Pawn->MaxHP;
		//Pawn->CurrentState = EBattleCharacterState::Normal;
		Pawn->NetMulticast_ReSetting(SpawnLocation);
	}
}
