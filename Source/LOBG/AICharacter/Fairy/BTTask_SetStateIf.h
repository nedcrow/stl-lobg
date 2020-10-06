// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "FairyPawn.h"
#include "BTTask_SetStateIf.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ECondition : uint8 {
	LessThenDistance	 = 0	UMETA(Display = "< Distance"),
	GreaterThenDistance	 = 1	UMETA(Display = "> Distance"),

};

UCLASS()
class LOBG_API UBTTask_SetStateIf : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Condition")
	ECondition TargetCondition;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Condition")
	float TargetDistance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Condition")
	EFairyState TargetState;

};
