// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePS.h"
#include "Net/UnrealNetwork.h"
#include "BattlePC.h"
#include "Kismet/GameplayStatics.h"
#include "BattleWidgetBase.h"

void ABattlePS::OnRep_Exp()
{
	if (Exp >= NextExp)
	{
		NextExp = Exp * 2;
		Level += 1;
	}
}

void ABattlePS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABattlePS, Level);
	DOREPLIFETIME(ABattlePS, Money);
	DOREPLIFETIME(ABattlePS, Exp);
}
