// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_TurnToTarget.h"

#include "../AIMinionChar.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UBTService_TurnToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (AIC && AIC->GetBlackboardComponent())
	{
		//AActor* tmpPawn = Cast<AActor>(AIC->BBComponent->GetValueAsObject(TEXT("Player")));
		AAIMinionChar* Minion = AIC->GetPawn<AAIMinionChar>();
		if (Minion)
		{
			AActor* TargetPawn = Cast<AActor>(AIC->GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));
			if (TargetPawn && !TargetPawn->IsActorBeingDestroyed())
			{
				// 방향 계산
				FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(Minion->GetActorLocation(), TargetPawn->GetActorLocation()).Clamp();

				// 액터 요 보간.
				if (!FMath::IsNearlyEqual(FRotator::ClampAxis(Minion->GetActorRotation().Yaw), LookAt.Yaw, 1.f))
				{
					Minion->SetActorRotation(FMath::RInterpTo(Minion->GetActorRotation(), FRotator(0.f, LookAt.Yaw, 0.f), DeltaSeconds, 5.f));
				}

				// 컨트롤러 핏치 보간. 에임오프셋에 사용.			
				if ((LookAt.Pitch > 270.5f || LookAt.Pitch < 89.5f) && !FMath::IsNearlyEqual(Minion->AimPitch, LookAt.Pitch, 1.f))
				{
					if (LookAt.Pitch > 270.5f) LookAt.Pitch -= 360.f;

					Minion->AimPitch = FMath::FInterpTo(Minion->AimPitch, LookAt.Pitch, DeltaSeconds, 2.f);
				}
			}
			else
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("TargetPawn"), nullptr);
				Minion->SetState(EMinioonState::Normal);
			}
		}
	}
}
