// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePS.h"
#include "Net/UnrealNetwork.h"
#include "BattlePC.h"
#include "Kismet/GameplayStatics.h"
#include "BattleWidgetBase.h"
#include "BattleCharacter.h"
#include "../Store/StoreWidgetBase.h"
#include "../Store/StoreItemBoxWidgetBase.h"
#include "BattleGS.h"

void ABattlePS::OnRep_Exp()
{
	if (PlayerExp >= NextExp)
	{
		SetNextExp();
		PlayerLevel += 1;
		MyPlayerData.PlayerLevel += 1;
		ABattlePC* PC = Cast<ABattlePC>(GetOwner());
		if (PC)
		{
			PC->UpdateGSTabArrayData();
		}

		NewExp = 0;
		OnRep_Level();
	}

	ABattlePC* PC = Cast<ABattlePC>(GetOwner());
	if (PC && PC->IsLocalController() && PC->BattleWidgetObject)
	{
		float totalExp = NextExp - GetStartExpInCurrentLevel();
		PC->BattleWidgetObject->SetExpBar(NewExp / totalExp);
	}

}

void ABattlePS::BeginPlay()
{
	SetNextExp();
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
		//PC->StoreWidgetObject->ItemBox->CheckAllSlotActive();
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
	DOREPLIFETIME(ABattlePS, FireBulletAngle);
	DOREPLIFETIME(ABattlePS, PlayerFOV);
	DOREPLIFETIME(ABattlePS, MaxHP);
	DOREPLIFETIME(ABattlePS, BulletSpeed);
	//DOREPLIFETIME(ABattlePS, GunDataArray);
}

void ABattlePS::SetNextExp()
{
	LevelIndex++;
	FLevelStruct * LevelTable = LevelDataTable->FindRow<FLevelStruct>(FName(*(FString::FormatAsNumber(LevelIndex))), TEXT(""));
	if (LevelTable)
	{
		NextExp = (*LevelTable).ExpforNextLevel;
	}
}

int ABattlePS::GetStartExpInCurrentLevel()
{
	FLevelStruct * LevelTable = LevelDataTable->FindRow<FLevelStruct>(FName(*(FString::FormatAsNumber(LevelIndex))), TEXT(""));
	if (LevelTable)
	{
		return (*LevelTable).Exp;
	}
	return 0;
}
