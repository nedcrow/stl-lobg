// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChangeTargetLocation.h"

#include "../../Battle/BattleGM.h"
#include "../AIManager.h"

EBTNodeResult::Type UBTTask_ChangeTargetLocation::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	// AI매니저를 ??? 에서 받아온다.
	ABattleGM * GM = GetWorld()->GetAuthGameMode<ABattleGM>();
	if (GM && GM->AIManagers.Num() > 0)
	{
		if (OwnerComp.IsValidLowLevelFast() && OwnerComp.GetAIOwner())
		{
			// AI매니저에 폰을 보낸다. AI매니저->함수(폰)
			// AI매니저에서 폰의 현재 목적지 정보를 받아서 상대 좌표를 찾고 폰의 현재 목적지를 변경한다.
			// 상대 좌표를 BB에 기록한다.
			// BB 로케이션 키에 입력.

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
