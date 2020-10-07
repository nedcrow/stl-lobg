// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MinionAttack.h"

#include "../AIMinionChar.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_MinionAttack::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	if (OwnerComp.GetAIOwner())
	{
		AAIMinionChar* AIChar = OwnerComp.GetAIOwner()->GetPawn<AAIMinionChar>();
		if (AIChar)
		{
			AActor* TargetPawn = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetPawn")));
			if (TargetPawn)
			{
				// 타겟 액터 or 로케이션
				AIChar->OnFire(TargetPawn->GetActorLocation());

				return EBTNodeResult::Succeeded;
			}
		}
	}






	return EBTNodeResult::Failed;
}
