// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_CheckDistance.h"

#include "../AIMinionChar.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_CheckDistance::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{

	if (OwnerComp.GetAIOwner())
	{
		AAIMinionChar* AIPawn = OwnerComp.GetAIOwner()->GetPawn<AAIMinionChar>();
		if (AIPawn)
		{
			AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetPawn")));
			if (TargetActor)
			{
				float Distance = FVector::Distance(AIPawn->GetActorLocation(), TargetActor->GetActorLocation());

				switch (TargetCondition)
				{
				case EArithmeticKeyOperation::Equal:
					if (Distance == TargetDistance)
					{
						AIPawn->SetState(TargetState);
						return EBTNodeResult::Succeeded;
					}
					break;
				case EArithmeticKeyOperation::NotEqual:
					if (Distance != TargetDistance)
					{
						AIPawn->SetState(TargetState);
						return EBTNodeResult::Succeeded;
					}
					break;
				case EArithmeticKeyOperation::Less:
					if (Distance < TargetDistance)
					{
						AIPawn->SetState(TargetState);
						return EBTNodeResult::Succeeded;
					}
					break;
				case EArithmeticKeyOperation::LessOrEqual:
					if (Distance <= TargetDistance)
					{
						AIPawn->SetState(TargetState);
						return EBTNodeResult::Succeeded;
					}
					break;
				case EArithmeticKeyOperation::Greater:
					if (Distance > TargetDistance)
					{
						AIPawn->SetState(TargetState);
						return EBTNodeResult::Succeeded;
					}
					break;
				case EArithmeticKeyOperation::GreaterOrEqual:
					if (Distance >= TargetDistance)
					{
						AIPawn->SetState(TargetState);
						return EBTNodeResult::Succeeded;
					}
					break;
				default:
					break;
				}
			}
		}
	}



	return EBTNodeResult::Failed;
}
