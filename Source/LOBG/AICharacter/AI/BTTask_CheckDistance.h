// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "../AIMinionChar.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"

#include "BTTask_CheckDistance.generated.h"

/**
 *
 */
UCLASS()
class LOBG_API UBTTask_CheckDistance : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		TEnumAsByte<EArithmeticKeyOperation::Type> TargetCondition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float TargetDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		EMinioonState TargetState;
};
