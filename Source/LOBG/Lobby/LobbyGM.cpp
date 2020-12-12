// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGM.h"
#include "LobbyGS.h"
#include "LobbyPC.h"
#include "UI/LobbyWidgetBase.h"
#include "../LOBGGameInstance.h"

void ALobbyGM::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	CountConnect();
}

void ALobbyGM::Logout(AController* Exiting)
{
	CountConnect();
	Super::Logout(Exiting);
}

void ALobbyGM::BeginPlay()
{
	Super::BeginPlay();

}

void ALobbyGM::StartGame()
{
	ULOBGGameInstance* GI = GetGameInstance<ULOBGGameInstance>();
	if (GI)
	{
		GI->TeamRedUsers = TeamRedNameArray;
		GI->TeamBlueUsers = TeamBlueNameArray;
	}
	GetWorld()->ServerTravel(TEXT("Step03_BattleTest3"));
}

void ALobbyGM::CountConnect()
{
	ALobbyGS* GS = GetGameState<ALobbyGS>();
	if (GS)
	{
		GS->ConnectCount = 0;
		for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; Iter++)
		{
			GS->ConnectCount++;
			GS->OnRepConnectCount();
		}
	}
}

void ALobbyGM::MakeTeam(const FString& UserName)
{
	//추가된 플레이어의 이름을 배열에 추가
	if (TeamRedNameArray.Num() <= TeamBlueNameArray.Num())
	{
		// Red에 넣기
		TeamRedNameArray.Emplace(UserName);
	}
	else
	{
		// Blue에 넣기
		TeamBlueNameArray.Emplace(UserName);
	}

	for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; ++Iter)
	{
		ALobbyPC* PC = Cast<ALobbyPC>(*Iter);
		if (PC)
		{
			//if(PC->LobbyWidgetObject)로 검사하는거 하면 실행안됨 왜?
			//서버에 있는 PC라서 LobbyWidgetObject가 없기 때문에

			//각 PC에 슬롯 동기화
			PC->Client_SplitTeam(TeamRedNameArray, TeamBlueNameArray);
		}
	}
}

void ALobbyGM::ChangeTeam(const FString& UserName)
{
	for (int i = 0; i < TeamRedNameArray.Num(); ++i)
	{
		if (TeamRedNameArray[i] == UserName)
		{
			TeamRedNameArray.RemoveSingle(UserName);
			TeamBlueNameArray.Emplace(UserName);
			for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; ++Iter)
			{
				ALobbyPC* PC = Cast<ALobbyPC>(*Iter);
				if (PC)
				{
					PC->Client_SplitTeam(TeamRedNameArray, TeamBlueNameArray);
				}
			}
			return;
		}
	}

	for (int i = 0; i < TeamBlueNameArray.Num(); ++i)
	{
		if (TeamBlueNameArray[i] == UserName)
		{
			TeamBlueNameArray.RemoveSingle(UserName);
			TeamRedNameArray.Emplace(UserName);
			for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; ++Iter)
			{
				ALobbyPC* PC = Cast<ALobbyPC>(*Iter);
				if (PC)
				{
					PC->Client_SplitTeam(TeamRedNameArray, TeamBlueNameArray);
				}
			}
			return;
		}
	}
}
