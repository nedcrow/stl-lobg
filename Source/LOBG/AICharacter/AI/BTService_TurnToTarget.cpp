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
		AActor* TargetPawn = Cast<AActor>(AIC->GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));
		AAIMinionChar* Minion = AIC->GetPawn<AAIMinionChar>();
		if (TargetPawn || Minion)
		{
			FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(Minion->GetActorLocation(), TargetPawn->GetActorLocation()).Clamp();
			
			//Zombie->SetActorRotation(LookAt);
			//float YawDiff = ((TargetPawn->GetActorLocation() - Minion->GetActorLocation()).Rotation() - Minion->GetActorRotation()).Clamp().Yaw;

			// ���� �� ����.
			if (!FMath::IsNearlyEqual(Minion->GetActorRotation().Yaw, LookAt.Yaw, 1.f))
			{
				Minion->SetActorRotation(FMath::RInterpTo(Minion->GetActorRotation(), FRotator(0.f, LookAt.Yaw, 0.f), DeltaSeconds, 5.f));
			}

			// ��Ʈ�ѷ� ��ġ ����. ���ӿ����¿� ���.
			if (!(LookAt.Pitch > 89.f && LookAt.Pitch < 271.f) && !FMath::IsNearlyEqual(AIC->GetControlRotation().Pitch, LookAt.Pitch, 1.f))
			{
				Minion->AimPitch = FMath::ClampAngle(LookAt.Pitch, -89.f, 89.f);
			}
			

			//if (YawDiff > 5.f)
			//{
			//	Minion->AddControllerYawInput(1.f*DeltaSeconds);
			//}
			//AIC->SetControlRotation(FMath::RInterpTo(Minion->GetActorRotation(), LookAt, DeltaSeconds, 1.f));
		}
	}

}