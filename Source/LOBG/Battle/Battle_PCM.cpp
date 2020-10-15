// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle_PCM.h"

#include "BattleCharacter.h"

void ABattle_PCM::UpdateCamera(float DeltaTime)
{
	APlayerController* PC = GetOwningPlayerController();
	if (PC && PC->IsValidLowLevelFast())
	{
		ABattleCharacter* Pawn = Cast<ABattleCharacter>(PC->GetPawn());
		if (Pawn && Pawn->IsValidLowLevelFast())
		{
			TargetFOV = Pawn->bIsIronsight ? IronsightFOV : NormalFOV;
			if (GetFOVAngle() != TargetFOV)
			{
				SetFOV(FMath::FInterpTo(GetFOVAngle(), TargetFOV, DeltaTime, 15.0f));
			}
		}
	}



	Super::UpdateCamera(DeltaTime);
}
