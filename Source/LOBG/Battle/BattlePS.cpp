// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePS.h"
#include "Net/UnrealNetwork.h"
#include "BattlePC.h"
#include "Kismet/GameplayStatics.h"
#include "BattleWidgetBase.h"
#include "BattleCharacter.h"
#include "../Store/StoreWidgetBase.h"
#include "BattleGS.h"

void ABattlePS::OnRep_Exp()
{
	if (PlayerExp >= NextExp)
	{
		NextExp = PlayerExp * 2.f;
		PlayerLevel += 1;
		NewExp = 0;
		OnRep_Level();
		TempExp = PlayerExp;
	}

	ABattlePC* PC = Cast<ABattlePC>(GetOwner());
	if (PC && PC->IsLocalController() && PC->BattleWidgetObject)
	{
		float totalExp = NextExp - TempExp;
		PC->BattleWidgetObject->SetExpBar(NewExp / totalExp);
		UE_LOG(LogClass, Warning, TEXT("New Exp : %f, TotalExp : %f"), NewExp, totalExp);
	}

}

void ABattlePS::OnRep_Level()
{
	ABattlePC* PC = Cast<ABattlePC>(GetOwner());
	if (PC && PC->IsLocalController() && PC->BattleWidgetObject)
	{
		PC->BattleWidgetObject->SetLevel(PlayerLevel);
	}
}

void ABattlePS::OnRep_Money()
{
	ABattlePC* PC = Cast<ABattlePC>(GetOwner());
	if (PC && PC->IsLocalController())
	{
		PC->BattleWidgetObject->SetMoney(PlayerMoney);
		PC->StoreWidgetObject->CheckSlotActive();
	}
}

void ABattlePS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABattlePS, PlayerLevel);
	DOREPLIFETIME(ABattlePS, PlayerMoney);
	DOREPLIFETIME(ABattlePS, PlayerExp);
	DOREPLIFETIME(ABattlePS, AttackPoint);
	DOREPLIFETIME(ABattlePS, PlayerSpeed);
	DOREPLIFETIME(ABattlePS, NewExp);
	DOREPLIFETIME(ABattlePS, TeamColor);
	DOREPLIFETIME(ABattlePS, PlayerMeshType);
}
