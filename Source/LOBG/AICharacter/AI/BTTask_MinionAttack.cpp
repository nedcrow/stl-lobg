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
			if (TargetPawn && !TargetPawn->IsActorBeingDestroyed())
			{
				// Ÿ�� ���� or �����̼�. ��ü�� �����̼��� �����Ѵ�.
				AIChar->OnFire(TargetPawn->GetActorLocation() + FVector(0.f, 0.f, 20.f));

				return EBTNodeResult::Succeeded;
			}
			else
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("TargetPawn"), nullptr);
				AIChar->SetState(EMinioonState::Normal);
			}
		}
	}






	return EBTNodeResult::Failed;
}
