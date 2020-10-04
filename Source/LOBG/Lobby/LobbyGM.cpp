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
	GetWorld()->ServerTravel(TEXT("Step03_LOBG"));
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
	//�߰��� �÷��̾��� �̸��� �迭�� �߰�
	UserNameArray.Emplace(UserName);

	for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; ++Iter)
	{
		ALobbyPC* PC = Cast<ALobbyPC>(*Iter);
		if (PC)
		{
			//if(PC->LobbyWidgetObject)�� �˻��ϴ°� �ϸ� ����ȵ� ��?
			//������ �ִ� PC�� LobbyWidgetObject�� ���� ������

			//�� PC�� ���� ����ȭ
			PC->Client_SplitTeam(UserNameArray);
		}
	}

}
