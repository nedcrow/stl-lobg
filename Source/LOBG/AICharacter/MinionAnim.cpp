// Fill out your copyright notice in the Description page of Project Settings.


#include "MinionAnim.h"

#include "AIMinionChar.h"

#include "GameFramework/CharacterMovementComponent.h"

void UMinionAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	AAIMinionChar* MinionPawn = Cast<AAIMinionChar>(TryGetPawnOwner());
	if (MinionPawn && MinionPawn->IsValidLowLevelFast())
	{
		// State
		CurrentState = MinionPawn->CurrentState;

		// Move
		MoveSpeed = MinionPawn->GetCharacterMovement()->Velocity.Size();

		// IsFire
		bIsFire = MinionPawn->bIsFire;

		// AimPitch
		AimPitch = MinionPawn->AimPitch;

	}
}