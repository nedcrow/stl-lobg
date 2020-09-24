// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"
#include "BattleCharacter.h"

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	ABattleCharacter* Pawn = Cast<ABattleCharacter>(TryGetPawnOwner());
	if (Pawn && Pawn->IsValidLowLevel()) {
		// bIsReload = Pawn->bIsReload;
		// bIsReload 참이고, Montage 있고, Montage가 실행중 아니면 ReloadMontage 실행
	}
}

void UCharacterAnimInstance::AnimNotify_ReloadEnd(UAnimNotify * AnimNotify)
{
	UE_LOG(LogClass, Warning, TEXT("Reload End Notify"));
	ABattleCharacter* Pawn = Cast<ABattleCharacter>(TryGetPawnOwner());
	if (Pawn && Pawn->IsValidLowLevel())
	{
		bIsReload = false;
		// Pawn->bIsReload = false;
		// Pawn->Client_SetReload(false);
	}
}
