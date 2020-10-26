// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_CallFire.h"
#include "../Fairy/FairyAIController.h"
#include "../Fairy/FairyPawn.h"
#include "../AIMinionChar.h"
#include "../../Battle/BattleCharacter.h"
#include "BehaviorTree/BlackBoardComponent.h"

EBTNodeResult::Type UBTTask_CallFire::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AFairyAIController* AIC = Cast<AFairyAIController>(OwnerComp.GetAIOwner());
	if (AIC)
	{
		AFairyPawn* Pawn = AIC->GetPawn<AFairyPawn>();
		AActor* Enemy = Cast<AActor>(AIC->BBComponent->GetValueAsObject(TEXT("Enemy")));
		
		if (Pawn && Enemy && Enemy != NULL)
		{
			if (Enemy && Enemy != NULL) {
				// dead state checking.
				if (Enemy->ActorHasTag("Minion")) {
					AAIMinionChar* Actor = Cast<AAIMinionChar>(Enemy);
					if (Actor && Actor->CurrentState == EMinioonState::Dead) {
						Pawn->SetCurrentState(EFairyState::Idle);
						return EBTNodeResult::Failed;
					}
				}
				else if (Enemy->ActorHasTag("Player")) {
					ABattleCharacter* Actor = Cast<ABattleCharacter>(Enemy);
					if (Actor && Actor->CurrentState == EBattleCharacterState::Dead) {
						Pawn->SetCurrentState(EFairyState::Idle);
						return EBTNodeResult::Failed;
					}
				}
				Pawn->StartFireTo(Enemy->GetTransform().GetLocation());
			}
		}
	}

	return EBTNodeResult::Succeeded;
}