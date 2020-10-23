// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGS.h"
#include "Net/UnrealNetwork.h"
#include "BattlePC.h"
#include "Kismet/GameplayStatics.h"
#include "BattleWidgetBase.h"
#include "BattleGM.h"
#include "../UI/GameStartWidgetBase.h"

void ABattleGS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABattleGS, GameStartTime);
	DOREPLIFETIME(ABattleGS, RedTabDataArray);
	DOREPLIFETIME(ABattleGS, BlueTabDataArray);
}

void ABattleGS::BeginPlay()
{
	Super::BeginPlay();

	//GameStartTime = 5;
	if (HasAuthority())
	{
		//GetWorldTimerManager().SetTimer(GameTimerHandle, this, &ABattleGS::SetGameStartTime, 1.f, false);
	}
	//if (GetWorld()->IsServer())
	//{
	//	
	//}
}

void ABattleGS::OnRep_GameStartTime()
{
	ABattlePC* PC = Cast<ABattlePC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC && PC->GameStartWidgetObject)
	{
		PC->GameStartWidgetObject->SetGameStartTimeText(GameStartTime);

		if (GameStartTime == 0)
		{
			PC->GameStartWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
