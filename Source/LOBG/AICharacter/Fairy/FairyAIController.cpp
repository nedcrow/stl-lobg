// Fill out your copyright notice in the Description page of Project Settings.


#include "FairyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "FairyPawn.h"
#include "BehaviorTree/BehaviorTree.h"

AFairyAIController::AFairyAIController() {
	BTComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BTComponent"));
	BBComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BBComponent"));
}

void AFairyAIController::OnPossess(APawn * InPawn)
{
	Super::OnPossess(InPawn);

	AFairyPawn* FairyPawn = Cast<AFairyPawn>(InPawn);
	if (FairyPawn) {
		if (FairyPawn->BTFairy && FairyPawn->BTFairy->BlackboardAsset) {
			BBComponent->InitializeBlackboard(*(FairyPawn->BTFairy->BlackboardAsset));
			BTComponent->StartTree(*(FairyPawn->BTFairy));
		}
	}
}

void AFairyAIController::OnUnPossess()
{
	BTComponent->StopTree();
	Super::OnUnPossess();
}

void AFairyAIController::SetCurrentState(EFairyState EState)
{
	if (BBComponent)
	{
		BBComponent->SetValueAsEnum(TEXT("CurrentState"), (uint8)EState);
	}
}

void AFairyAIController::SetEnemy(AActor * Enemy)
{
	if (BBComponent)
	{
		BBComponent->SetValueAsObject(TEXT("Enemy"), Enemy);
		CurrentEnermy = Enemy;
	}
}
