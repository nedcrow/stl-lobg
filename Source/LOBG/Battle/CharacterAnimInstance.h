// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class LOBG_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		float Speed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		float Direction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bIsFire : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bIsSprint : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bIsIronsight : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bIsCrouched : 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bIsInAir : 1;

	// lean
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bIsLeanLeft : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bIsLeanRight : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		float CurrentLeanAngle;

	// reload
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bIsReload : 1;

	// Aim
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		float AimPitch;

	UFUNCTION()
		void AnimNotify_ReloadEnd(UAnimNotify* AnimNotify);
};
