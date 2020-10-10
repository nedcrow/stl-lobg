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
	ULOBGGameInstance* GI = GetGameInstance<ULOBGGameInstance>();
	if (GI)
	{
		UE_LOG(LogClass, Warning, TEXT("BattleGM !!!!!!%d %d"), GI->TeamRedUsers.Num(), GI->TeamBlueUsers.Num());
		TeamRedUsers = GI->TeamRedUsers;
		TeamBlueUsers = GI->TeamBlueUsers;
	}
	UE_LOG(LogClass, Warning, TEXT("BattleGM !!!!!!%d %d"), TeamRedUsers.Num(), TeamBlueUsers.Num());

	FindPlayerStart();

	FTimerHandle PlayerTimer;
	GetWorldTimerManager().SetTimer(PlayerTimer, this, &ABattleGM::PlayerSpawn, 2.0f, false);
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
	for (auto Iter = GetWorld()->GetControllerIterator(); Iter; ++Iter)
	{
		ABattlePC* PC = Cast<ABattlePC>(*Iter);
		if (PC)
		{
			PC->Clinet_SetTeamColorInPC();
		}
	}
}

void ABattleGM::TestPlayerSpawn(ETeamColor newcolor, ABattlePC* NewPC)
{
	FName TagText;
	if (newcolor == ETeamColor::Red)
	{
		TagText = TEXT("Red");
	}
	else if (newcolor == ETeamColor::Blue)
	{
		TagText = TEXT("Blue");
	}

	ABattlePC* PC = Cast<ABattlePC>(NewPC);
	if (PC)
	{
		for (int i = 0; i < OutputPlayerStart.Num(); ++i)
		{
			if (OutputPlayerStart[i]->ActorHasTag(TagText))
			{
				PC->Client_TestWidget();
				//float Random = FMath::RandRange(10.f, 30.f);
				//FVector Location = FVector(OutputPlayerStart[i]->GetActorLocation().X + Random, OutputPlayerStart[i]->GetActorLocation().Y + Random, OutputPlayerStart[i]->GetActorLocation().Z);
				ABattleCharacter* PlayerPawn = GetWorld()->SpawnActor<ABattleCharacter>(
					PlayerClass, OutputPlayerStart[i]->GetActorLocation(), OutputPlayerStart[i]->GetActorRotation());
				PC->Possess(PlayerPawn);
				PC->SetPSTeamColorAndSetPlayerTag(newcolor);
				OutputPlayerStart.RemoveAt(i);
				return;
			}
		}
	}
}

