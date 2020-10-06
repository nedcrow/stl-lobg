// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "FairyPawn.h"

#include "FairyAIController.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API AFairyAIController : public AAIController
{
	GENERATED_BODY()
public:
	AFairyAIController();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UBehaviorTreeComponent* BTComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UBlackboardComponent* BBComponent;

	class AActor* CurrentEnermy;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	void SetCurrentState(EFairyState EState);

	void SetEnemy(AActor* Enemy);
	
};
