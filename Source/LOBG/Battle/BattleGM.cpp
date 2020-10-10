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

		//������ ���� �ڵ� ���� �������� �ʰ� Test�ʿ��� ����� �� ĳ���͸� �����ϱ� ����
		//���߿� ���� ����
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
			//������ ��ġ ��������
			AReSpawn* ReSpawnObj = Cast<AReSpawn>(UGameplayStatics::GetActorOfClass(GetWorld(), AReSpawn::StaticClass()));
			FVector SpawnLocation = ReSpawnObj->ReSpawnArea->GetComponentLocation();
			FRotator SpawnRotator = FRotator(0, 0, 0);

			ABattleCharacter* BattlePlayer = GetWorld()->SpawnActor<ABattleCharacter>(PlayerClass, SpawnLocation, SpawnRotator);

			//�÷��̾���Ʈ�ѷ��� ����
			PC->Possess(BattlePlayer);

			//��Ʈ�ѷ� ���� �� ������ ��ϵ�
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
	//������ PS�� TeamColor�� ���� �÷��̾� ����
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
					//���߿����� ���� �������� �������� ���� else�ڵ�
					//���߿� ��������
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
	//���߿� PC�� ���� �˻��ؼ� �̸��� ����Ǿ��ִٸ����� ��������

	UE_LOG(LogClass, Warning, TEXT("SetPSTeamColor"));
	CheckControllerHasName.Add(true);
	int AllPlayerNum = TeamRedUsers.Num() + TeamBlueUsers.Num();
	if (CheckControllerHasName.Num() == AllPlayerNum || TestMapVersonSpawn)
	{
		//������ PC���� ���̵� �� ������ �ִٸ� PS�� �� ����
		SetPSTeamColor();
	}
}

