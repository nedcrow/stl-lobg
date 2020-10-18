// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"
#include "BattleCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	ABattleCharacter* Pawn = Cast<ABattleCharacter>(TryGetPawnOwner());
	if (Pawn && Pawn->IsValidLowLevel())
	{
		// Move animation
		Direction = CalculateDirection(Pawn->GetCharacterMovement()->Velocity, Pawn->GetActorRotation());
		Speed = Pawn->GetCharacterMovement()->Velocity.Size();

		//bIsFire = Pawn->bIsFire;
		bIsCrouched = Pawn->bIsCrouched;
		bIsIronsight = Pawn->bIsIronsight;


		// Reload - Test
		bIsReload = Pawn->bIsReload;
		// Montage 있고, Montage가 실행중 아니면 ReloadMontage 실행
		if (bIsReload)
		{
			switch (Pawn->PlayerMeshType)
			{
			case EMeshType::None:
				Pawn->CurrentReloadMontage = Pawn->ReloadMontage;
				break;
			case EMeshType::Female:
				Pawn->CurrentReloadMontage = Pawn->ReloadMontage_Female;
				break;
			case EMeshType::Male:
				Pawn->CurrentReloadMontage = Pawn->ReloadMontage_Male;
				break;
			default:
				break;
			}

			if (Pawn->CurrentReloadMontage && !Montage_IsPlaying(Pawn->CurrentReloadMontage))
			{
				Pawn->PlayAnimMontage(Pawn->CurrentReloadMontage);
			}
		}


		// SetCurrentAngle & Lean
		bIsLeanLeft = Pawn->bIsLeanLeft;
		bIsLeanRight = Pawn->bIsLeanRight;
		float TargetLeanAngle;
		if (Pawn->bIsLeanLeft && !Pawn->bIsLeanRight)
		{
			TargetLeanAngle = -40.0f;
		}
		else if (Pawn->bIsLeanRight && !Pawn->bIsLeanLeft)
		{
			TargetLeanAngle = 30.0f;
		}
		else
		{
			TargetLeanAngle = 0;
		}
		CurrentLeanAngle = FMath::FInterpTo(CurrentLeanAngle, TargetLeanAngle, DeltaSeconds, 8.0f);

		// AimOffset
		AimPitch = Pawn->GetAimOffset().Pitch;

		// Jump
		bIsInAir = Pawn->GetCharacterMovement()->IsFalling();
	}
}

void UCharacterAnimInstance::AnimNotify_ReloadEnd(UAnimNotify * AnimNotify)
{
	ABattleCharacter* Pawn = Cast<ABattleCharacter>(TryGetPawnOwner());
	if (Pawn && Pawn->IsValidLowLevel())
	{
		bIsReload = false;
		Pawn->bIsReload = false;
		Pawn->Server_SetReload(false);
	}
}
