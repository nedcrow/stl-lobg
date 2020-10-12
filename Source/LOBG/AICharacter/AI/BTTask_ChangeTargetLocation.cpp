// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChangeTargetLocation.h"

#include "../../Battle/BattleGM.h"
#include "../AIManager.h"

EBTNodeResult::Type UBTTask_ChangeTargetLocation::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	// AI�Ŵ����� ??? ���� �޾ƿ´�.
	ABattleGM * GM = GetWorld()->GetAuthGameMode<ABattleGM>();
	if (GM && GM->AIManagers.Num() > 0)
	{
		if (OwnerComp.IsValidLowLevelFast() && OwnerComp.GetAIOwner())
		{
			// AI�Ŵ����� ���� ������. AI�Ŵ���->�Լ�(��)
			// AI�Ŵ������� ���� ���� ������ ������ �޾Ƽ� ��� ��ǥ�� ã�� ���� ���� �������� �����Ѵ�.
			// ��� ��ǥ�� BB�� ����Ѵ�.
			// BB �����̼� Ű�� �Է�.

			AAIManager* AIM = GM->FindAIM(OwnerComp.GetAIOwner());
			if (AIM)
			{
				bool bResult = AIM->ChangeNextTarget(OwnerComp.GetAIOwner());

				if (bResult)
				{
					return EBTNodeResult::Succeeded;
				}
			}
		}
	}

	return EBTNodeResult::Failed;
}
