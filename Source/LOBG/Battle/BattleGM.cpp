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
		if (Pawn->CurrentState != EBattleCharacterState::Dead) return;
		
		ABattlePC* PC = Cast<ABattlePC>(Pawn->GetController());
		if (PC)
		{
			//������ ��ġ ��������
			AReSpawn* ReSpawnObj = Cast<AReSpawn>(UGameplayStatics::GetActorOfClass(GetWorld(), AReSpawn::StaticClass()));
			FVector SpawnLocation = ReSpawnObj->ReSpawnArea->GetComponentLocation();
			FRotator SpawnRotator = FRotator(0, 0, 0);

			ABattleCharacter* BattlePlayer = GetWorld()->SpawnActor<ABattleCharacter>(PlayerClass, SpawnLocation, SpawnRotator);

			//�÷��̾���Ʈ�ѷ��� ����
			PC->Possess(BattlePlayer);

			//PlayerState�� ����� ���� ����ȭ???
		}
	}
}
