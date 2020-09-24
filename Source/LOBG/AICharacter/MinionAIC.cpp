// Fill out your copyright notice in the Description page of Project Settings.


#include "MinionAIC.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

AMinionAIC::AMinionAIC()
{
	BTComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BTComponent"));
	BBComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BBComponent"));
}

void AMinionAIC::OnPossess(APawn * InPawn)
{
	Super::OnPossess(InPawn);

	AAIMinionChar* MinionPawn = Cast<AAIMinionChar>(GetPawn());
	if (MinionPawn && MinionPawn->BTMinion && MinionPawn->BTMinion->BlackboardAsset)
	{
			BBComponent->InitializeBlackboard(*MinionPawn->BTMinion->BlackboardAsset);
			BTComponent->StartTree(*MinionPawn->BTMinion);		
	}
}

void AMinionAIC::OnUnPossess()
{
	BTComponent->StopTree();

	Super::OnUnPossess();
}

void AMinionAIC::SetValueState(EMinioonState NewState)
{
	BBComponent->SetValueAsEnum(TEXT("MinionState"), (uint8)NewState);
}
