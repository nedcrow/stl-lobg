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
#include "GameFramework/PlayerStart.h"

void ABattleGM::BeginPlay()
{
	Super::BeginPlay();

	FindPlayerStart();

	//FTimerHandle PlayerTimer;
	//GetWorldTimerManager().SetTimer(PlayerTimer, this, &ABattleGM::PlayerSpawn, 5.0f, false);
	//PlayerSpawn();

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
	//PlayerSpawn();
}

void ABattleGM::CallReSpawn(ABattleCharacter* Pawn)
{
	if (Pawn)
	{
		if (Pawn->CurrentState != EBattleCharacterState::Dead) return;
		
		ABattlePC* PC = Cast<ABattlePC>(Pawn->GetController());
		if (PC)
		{
			//리스폰 위치 가져오기
			AReSpawn* ReSpawnObj = Cast<AReSpawn>(UGameplayStatics::GetActorOfClass(GetWorld(), AReSpawn::StaticClass()));
			FVector SpawnLocation = ReSpawnObj->ReSpawnArea->GetComponentLocation();
			FRotator SpawnRotator = FRotator(0, 0, 0);

			ABattleCharacter* BattlePlayer = GetWorld()->SpawnActor<ABattleCharacter>(PlayerClass, SpawnLocation, SpawnRotator);

			//플레이어컨트롤러에 연결
			PC->Possess(BattlePlayer);

			//컨트롤러 연결 후 리셋할 목록들
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

void ABattleGM::FindPlayerStart()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), OutputPlayerStart);
}

void ABattleGM::PlayerSpawn()
{
	int PlayerNumber = 0;
	for (auto Iter = GetWorld()->GetControllerIterator(); Iter; ++Iter)
	{
		//PlayerNumber = 0;
		PlayerNumber++;
	}
		UE_LOG(LogClass, Warning, TEXT("PlayerNumber is %d"), PlayerNumber);
	for (auto Iter = GetWorld()->GetControllerIterator(); Iter; ++Iter)
	{
		

		ABattlePC* PC = Cast<ABattlePC>(*Iter);
		if (PC)
		{
			PC->SetTeamColorInPC();
			//UE_LOG(LogClass, Warning, TEXT("TeamColor is "));
			for (int j = 0; j < OutputPlayerStart.Num(); ++j)
			{
				if (OutputPlayerStart[j]->ActorHasTag(TEXT("Red")))
				{
					if (PC->TestColor == ETeamColor::Red)
					{
						UE_LOG(LogClass, Warning, TEXT("Player Respawn in Red"));
						UGameplayStatics::CreatePlayer(GetWorld());
						ABattleCharacter* PlayerPawn = GetWorld()->SpawnActor<ABattleCharacter>(
							PlayerClass, OutputPlayerStart[j]->GetActorLocation(), OutputPlayerStart[j]->GetActorRotation());
						PC->Possess(PlayerPawn);
						PC->InitTeamColor();
						PC->TestWidget();
					}
				}
				else if (OutputPlayerStart[j]->ActorHasTag(TEXT("Blue")))
				{
					if (PC->TestColor == ETeamColor::Blue)
					{
						UE_LOG(LogClass, Warning, TEXT("Player Respawn in Blue"));
						ABattleCharacter* PlayerPawn = GetWorld()->SpawnActor<ABattleCharacter>(
							PlayerClass, OutputPlayerStart[j]->GetActorLocation(), OutputPlayerStart[j]->GetActorRotation());
						PC->Possess(PlayerPawn);
						PC->InitTeamColor();
						PC->TestWidget();
					}
				}
			}
			
		}

	}

	//for (int i = 0; i < PlayerNumber; ++i)
	//{
	//	for (int j = 0; j < OutputPlayerStart.Num(); ++j)
	//	{
	//		if(OutputPlayerStart[j]->ActorHasTag(TEXT("Red")))
	//		{
	//			UGameplayStatics::CreatePlayer(GetWorld(), i);
	//			ABattleCharacter* PlayerPawn = GetWorld()->SpawnActor<ABattleCharacter>(
	//				PlayerClass, OutputPlayerStart[j]->GetActorLocation(), OutputPlayerStart[j]->GetActorRotation());
	//			ABattlePC* PC = Cast<ABattlePC>(UGameplayStatics::GetPlayerController(GetWorld(), i));
	//			if (PC)
	//			{
	//				PC->Possess(PlayerPawn);
	//			}
	//		}
	//	}
	//}
}

