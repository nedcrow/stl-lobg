// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_CheckDistance.h"

//#include "../AIMinionChar.h"

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
			if (TargetActor && !TargetActor->IsActorBeingDestroyed())
			{
				float Distance = FVector::Distance(AIPawn->GetActorLocation(), TargetActor->GetActorLocation());

				if (TargetState == EMinioonState::Battle)
				{
					// 공격 전 방향 보정. 총구 위치를 예상 거리를 넣어서 방향을 보정한다.
					FRotator RelativeRotation = (TargetActor->GetActorLocation() + FVector(0.f, 0.f, 20.f) - AIPawn->GetActorLocation() - AIPawn->GetActorRightVector() * 30.f).Rotation();
					AIPawn->SetActorRotation(FRotator(0.f, RelativeRotation.Yaw, 0.f));
					AIPawn->AimPitch = RelativeRotation.Pitch;
				}

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
			else
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("TargetPawn"), nullptr);
				AIPawn->SetState(EMinioonState::Normal);
			}
		}
	}



	return EBTNodeResult::Failed;
}
