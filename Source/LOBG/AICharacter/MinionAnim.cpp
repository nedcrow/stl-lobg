// Fill out your copyright notice in the Description page of Project Settings.


#include "MinionAnim.h"

#include "GameFramework/CharacterMovementComponent.h"

void UMinionAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	AAIMinionChar* MinionPawn = Cast<AAIMinionChar>(TryGetPawnOwner());
	if (MinionPawn && MinionPawn->IsValidLowLevelFast())
	{
		// Move
		CurrentState = MinionPawn->CurrentState;

		MoveSpeed = MinionPawn->GetCharacterMovement()->Velocity.Size();
	}
}