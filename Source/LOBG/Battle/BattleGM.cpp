// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGM.h"
#include "BattleCharacter.h"
#include "../ReSpawn/ReSpawn.h"
#include "Kismet/GameplayStatics.h"

void ABattleGM::CallReSpawn(ABattleCharacter* Pawn)
{
	//아직 미완성임 건들지 말것
	if (Pawn)
	{
		if (Pawn->CurrentState != EBattleCharacterState::Dead) return;

		AReSpawn* ReSpawnObj = Cast<AReSpawn>(UGameplayStatics::GetActorOfClass(GetWorld(), AReSpawn::StaticClass()));
		FVector SpawnLocation = ReSpawnObj->ReSpawnArea->GetComponentLocation();
		SetReSpawnPawnSetting(Pawn, SpawnLocation);
	}
}

void ABattleGM::SetReSpawnPawnSetting_Implementation(ABattleCharacter* Pawn, FVector location)
{
	Pawn->SetActorLocation(location);
	Pawn->CurrentHP = Pawn->MaxHP;
	Pawn->CurrentState = EBattleCharacterState::Normal;
	Pawn->EnableInput(Cast<APlayerController>(Pawn->GetController()));
}
