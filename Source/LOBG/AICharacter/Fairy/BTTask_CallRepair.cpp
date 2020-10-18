// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_CallRepair.h"
#include "../Fairy/FairyAIController.h"
#include "../Fairy/FairyPawn.h"
#include "BehaviorTree/BlackBoardComponent.h"

EBTNodeResult::Type UBTTask_CallRepair::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AFairyAIController* AIC = Cast<AFairyAIController>(OwnerComp.GetAIOwner());
	if (AIC)
	{
		AFairyPawn* Pawn = AIC->GetPawn<AFairyPawn>();

		if (Pawn)
		{
			Pawn->Repair();
		}
	}

	return EBTNodeResult::Succeeded;
}