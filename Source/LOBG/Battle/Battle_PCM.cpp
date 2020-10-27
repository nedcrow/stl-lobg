// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle_PCM.h"
#include "BattlePC.h"
#include "BattleCharacter.h"
#include "BattlePS.h"

void ABattle_PCM::UpdateCamera(float DeltaTime)
{
	ABattlePC* PC = Cast<ABattlePC>(GetOwningPlayerController());
	if (PC && PC->IsValidLowLevelFast())
	{
		ABattleCharacter* Pawn = Cast<ABattleCharacter>(PC->GetPawn());
		if (Pawn && Pawn->IsValidLowLevelFast())
		{
			ABattlePS* PS = PC->GetPlayerState<ABattlePS>();
			if (PS)
			{
				TargetFOV = Pawn->bIsIronsight ? PS->PlayerFOV : NormalFOV;
			}
			if (GetFOVAngle() != TargetFOV)
			{
				
				SetFOV(FMath::FInterpTo(GetFOVAngle(), TargetFOV, DeltaTime, 15.0f));
			}
		}
	}



	Super::UpdateCamera(DeltaTime);
}
