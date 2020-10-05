// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "AIMinionChar.h"

#include "MinionAIC.generated.h"

/**
 *
 */
UCLASS()
class LOBG_API AMinionAIC : public AAIController
{
	GENERATED_BODY()

public:
	AMinionAIC();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	// AI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		class UBehaviorTreeComponent* BTComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		class UBlackboardComponent* BBComponent;

	// BB
	void SetValueState(EMinioonState NewState);
	void SetValueTargetPawn(APawn* NewPawn);
};
