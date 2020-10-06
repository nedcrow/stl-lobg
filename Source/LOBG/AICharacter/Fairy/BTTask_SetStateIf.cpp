// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetStateIf.h"
#include "../Fairy/FairyAIController.h"
#include "../Fairy/FairyPawn.h"
#include "BehaviorTree/BlackBoardComponent.h"

EBTNodeResult::Type UBTTask_SetStateIf::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AFairyAIController* AIC = Cast<AFairyAIController>(OwnerComp.GetAIOwner());
	if (AIC)
	{
		AFairyPawn* Pawn = AIC->GetPawn<AFairyPawn>();
		AActor* Enemy = Cast<AActor>(AIC->BBComponent->GetValueAsObject(TEXT("Enemy")));

		if (Pawn)
		{
			if (Enemy && Enemy != NULL) {
				float CurrentDistance = FVector::Distance(
					Pawn->GetActorLocation(),
					Enemy->GetActorLocation()
				);

				switch (TargetCondition)
				{
				case ECondition::LessThenDistance: // <
				{
					if (CurrentDistance < TargetDistance)
					{
						Pawn->SetCurrentState(TargetState);
					}
				}
				break;

				case ECondition::GreaterThenDistance: // >
				{
					if (CurrentDistance > TargetDistance)
					{
						Pawn->SetCurrentState(TargetState);
					}
				}
				break;
				}
			}
			else if(Pawn->CurrentState != EFairyState::Death) {
				Pawn->SetCurrentState(EFairyState::Idle);
			}
		}
	}


	return EBTNodeResult::Succeeded;
}