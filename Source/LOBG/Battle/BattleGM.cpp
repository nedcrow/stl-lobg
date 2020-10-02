// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGM.h"
#include "BattleCharacter.h"
#include "../ReSpawn/ReSpawn.h"
#include "Kismet/GameplayStatics.h"
#include "BattlePC.h"
#include "BattlePS.h"
#include "BattleGS.h"
#include "../AICharacter/AIManager.h"
#include "../Temp/TempTower.h"
#include "../LOBGGameInstance.h"

void ABattleGM::BeginPlay()
{
	Super::BeginPlay();


	// AIManager Spawn.
	if (AIManagerClass)
	{
		AAIManager * AIM = GetWorld()->SpawnActor<AAIManager>(AIManagerClass);
		if (AIM)
		{
			AIManager = AIM;

			if (SpawnAINumber > 0)
			{
				AIM->SetSpawnQuantity(SpawnAINumber);
			}
		}
	}
}

void ABattleGM::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	TArray<AActor*> OutTowers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TempTowerClass, OutTowers);
	TowerCount = OutTowers.Num();
	UE_LOG(LogClass, Warning, TEXT("TowerCount is %d"), TowerCount);

	//SplitTeam();
}

void ABattleGM::CallReSpawn(ABattleCharacter* Pawn)
{
	if (Pawn)
	{
		if (Pawn->CurrentState != EBattleCharacterState::Dead) return;
		
		ABattlePC* PC = Cast<ABattlePC>(Pawn->GetController());
		if (PC)
		{
			//������ ��ġ ��������
			AReSpawn* ReSpawnObj = Cast<AReSpawn>(UGameplayStatics::GetActorOfClass(GetWorld(), AReSpawn::StaticClass()));
			FVector SpawnLocation = ReSpawnObj->ReSpawnArea->GetComponentLocation();
			FRotator SpawnRotator = FRotator(0, 0, 0);

			ABattleCharacter* BattlePlayer = GetWorld()->SpawnActor<ABattleCharacter>(PlayerClass, SpawnLocation, SpawnRotator);

			//�÷��̾���Ʈ�ѷ��� ����
			PC->Possess(BattlePlayer);

			//��Ʈ�ѷ� ���� �� ������ ��ϵ�
			BattlePlayer->NetMulticast_ReSpawnUI();
		}
	}
}

void ABattleGM::CountTower()
{
	TowerCount--;
	UE_LOG(LogClass, Warning, TEXT("TowerCount is %d"), TowerCount);

	if (TowerCount == 0)
	{
		FTimerHandle GameOverTimer;
		GetWorldTimerManager().SetTimer(GameOverTimer, this, &ABattleGM::GoLobby, 3.f);
	}
}

void ABattleGM::GoLobby()
{
	GetWorld()->ServerTravel(TEXT("Step02_Lobby"));
}

void ABattleGM::SplitTeam()
{
	//RedPlayerNumber = 0;
	//BluePlayerNumber = 0;
	//for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; ++Iter)
	//{
	//	UE_LOG(LogClass, Warning, TEXT("GetPlayerControllerNumber"));
	//	ABattlePC* PC = Cast<ABattlePC>(*Iter);
	//	if (PC)
	//	{
	//		ABattleCharacter* Pawn = Cast<ABattleCharacter>(PC->GetPawn());
	//		if (Pawn)
	//		{
	//
	//			ABattlePS* PS = Pawn->GetPlayerState<ABattlePS>();
	//			if (PS)
	//			{
	//				if (PS && RedPlayerNumber != 2)
	//				{
	//					PS->TeamColor = ETeamColor::Red;
	//					RedPlayerNumber++;
	//					UE_LOG(LogClass, Warning, TEXT("RedPlayerNumber is %d"), RedPlayerNumber);
	//				}
	//				else if(PS)
	//				{
	//					PS->TeamColor = ETeamColor::Blue;
	//					BluePlayerNumber++;
	//					UE_LOG(LogClass, Warning, TEXT("BluePlayerNumber is %d"), BluePlayerNumber);
	//				}
	//			}
	//		}
	//		
	//	}
	//	
	//}
}

