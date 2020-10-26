// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MinionAttack.h"

#include "../AIMinionChar.h"
#include "../Fairy/FairyPawn.h"
#include "../../Battle/BattleCharacter.h"

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
				// 타겟 액터 or 로케이션. 상체의 로케이션을 전달한다. 현재 로케이션을 사용하지 않고 미니언의 사격 방향값을 사용중.
				if (TargetPawn->ActorHasTag("Minion")) {
					AAIMinionChar* Actor = Cast<AAIMinionChar>(TargetPawn);
					if (Actor && Actor->CurrentState == EMinioonState::Dead) {
						AIChar->SetState(EMinioonState::Normal);
						return EBTNodeResult::Failed;
					}
				}
				else if (TargetPawn->ActorHasTag("Player")) {
					ABattleCharacter* Actor = Cast<ABattleCharacter>(TargetPawn);
					if (Actor && Actor->CurrentState == EBattleCharacterState::Dead) {
						AIChar->SetState(EMinioonState::Normal);
						return EBTNodeResult::Failed;
					}
				}
				else if (TargetPawn->ActorHasTag("Tower")) {
					AFairyPawn* Actor = Cast<AFairyPawn>(TargetPawn);
					if (Actor && Actor->CurrentState == EFairyState::Death) {
						AIChar->SetState(EMinioonState::Normal);
						return EBTNodeResult::Failed;
					}
				}

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
