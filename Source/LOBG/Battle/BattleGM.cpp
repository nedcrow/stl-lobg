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
		TeamRedUsers = GI->TeamRedUsers;
		TeamBlueUsers = GI->TeamBlueUsers;

		//개발을 위한 코드 팀을 설정하지 않고 Test맵에서 사용할 때 캐릭터를 스폰하기 위함
		//나중에 지울 예정
		if (TeamRedUsers.Num() + TeamBlueUsers.Num() == 0)
		{
			TestMapVersonSpawn = true;
		}
	}
	FindPlayerStart();

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
			PC->InitPlayerWithTeam();
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

void ABattleGM::SetPSTeamColor()
{
	for (auto Iter = GetWorld()->GetControllerIterator(); Iter; ++Iter)
	{
		ABattlePC* PC = Cast<ABattlePC>(*Iter);
		if (PC)
		{
			for (int i = 0; i < TeamRedUsers.Num(); ++i)
			{
				if (PC->MyUserName == TeamRedUsers[i])
				{
					ABattlePS* PS = PC->GetPlayerState<ABattlePS>();
					if (PS)
					{
						PS->TeamColor = ETeamColor::Red;
						break;
					}
				}
			}
			
			for (int i = 0; i < TeamBlueUsers.Num(); ++i)
			{
				if (PC->MyUserName == TeamBlueUsers[i])
				{
					ABattlePS* PS = PC->GetPlayerState<ABattlePS>();
					if (PS)
					{
						PS->TeamColor = ETeamColor::Blue;
						break;
					}
				}
			}
		}
	}
	//설정한 PS의 TeamColor를 토대로 플레이어 스폰
	PlayerSpawn();
}

void ABattleGM::PlayerSpawn()
{
	for (auto Iter = GetWorld()->GetControllerIterator(); Iter; ++Iter)
	{
		ABattlePC* PC = Cast<ABattlePC>(*Iter);
		if (PC)
		{
			ABattlePS* PS = PC->GetPlayerState<ABattlePS>();
			if (PS)
			{
				FName TagText;
				if (PS->TeamColor == ETeamColor::Red)
				{
					TagText = TEXT("Red");
				}
				else if (PS->TeamColor == ETeamColor::Blue)
				{
					TagText = TEXT("Blue");
				}

				for (int i = 0; i < OutputPlayerStart.Num(); ++i)
				{
					if (OutputPlayerStart[i]->ActorHasTag(TagText))
					{
						PC->Client_TestWidget();
						ABattleCharacter* PlayerPawn = GetWorld()->SpawnActor<ABattleCharacter>(
							PlayerClass, OutputPlayerStart[i]->GetActorLocation(), OutputPlayerStart[i]->GetActorRotation());
						PC->Possess(PlayerPawn);
						PC->InitPlayerWithTeam();
						OutputPlayerStart.RemoveAt(i);
						break;
					}
					//개발용으로 팀을 설정하지 않을때를 위한 else코드
					//나중에 지워야함
					else if(TestMapVersonSpawn)
					{
						PC->Client_TestWidget();
						ABattleCharacter* PlayerPawn = GetWorld()->SpawnActor<ABattleCharacter>(
							PlayerClass, OutputPlayerStart[i]->GetActorLocation(), OutputPlayerStart[i]->GetActorRotation());
						PC->Possess(PlayerPawn);
						break;
					}
				}
			}
		}
	}
}

void ABattleGM::CheckAllControllerHasName()
{
	//나중에 PC를 직접 검사해서 이름이 저장되어있다면으로 수정가능

	UE_LOG(LogClass, Warning, TEXT("SetPSTeamColor"));
	CheckControllerHasName.Add(true);
	int AllPlayerNum = TeamRedUsers.Num() + TeamBlueUsers.Num();
	if (CheckControllerHasName.Num() == AllPlayerNum || TestMapVersonSpawn)
	{
		//서버의 PC들이 아이디를 다 가지고 있다면 PS에 팀 설정
		SetPSTeamColor();
	}
}

