// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "AIMinionChar.h"

#include "MinionAnim.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API UMinionAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
		EMinioonState CurrentState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
		float MoveSpeed;

	// Fire
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bIsFire : 1;

	// Aim
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		float AimPitch;
};
