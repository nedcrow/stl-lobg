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
#include "AIController.h"

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
		for (int i = 0; i < 2; i++)
		{
			AAIManager * AIM = GetWorld()->SpawnActor<AAIManager>(AIManagerClass);
			if (AIM)
			{
				AIManagers.Emplace(AIM);

				switch (i)
				{
				case 0:
					AIM->TeamName = TEXT("Red");
					break;
				case 1:
					AIM->TeamName = TEXT("Blue");
					AIM->ReverseCoursePoints(AIM->CoursePoints);		// �̵� ���� ������.
					break;
				default:
					break;
				}
			}
		}


		if (SpawnAINumber > 0)
		{
			RepeatMinionsWave();
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

// AI�Ŵ���
AAIManager* ABattleGM::FindAIM(AAIController * AIPawnOwner)
{
	if (AIPawnOwner && AIManagers.Num() > 0)
	{
		APawn* AIPawn = AIPawnOwner->GetPawn();
		if (AIPawn)
		{
			for (int i = 0; i < AIManagers.Num(); i++)
			{
				if (AIPawn->ActorHasTag(AIManagers[i]->TeamName))		// AI��Ʈ�ѷ��� ���� ���� ���� AI�Ŵ��� ã��.
				{
					return AIManagers[i];
				}
			}
		}
	}

	return nullptr;
}

void ABattleGM::RepeatMinionsWave()
{
	if (AIManagers.Num() > 0 && SpawnAINumber > 0)
	{
		for (int i = 0; i < AIManagers.Num(); i++)
		{
			AIManagers[i]->SetSpawnQuantity(SpawnAINumber);
		}

		GetWorldTimerManager().SetTimer(RepeatWaveHandle, this, &ABattleGM::RepeatMinionsWave, WaveRepeatSeconds, false);
	}
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
			FVector SpawnLocation = FVector(0.f, 0.f, 1000.f);
			AReSpawn* ReSpawnObj = Cast<AReSpawn>(UGameplayStatics::GetActorOfClass(GetWorld(), AReSpawn::StaticClass()));
			if (ReSpawnObj)
			{
				SpawnLocation = ReSpawnObj->ReSpawnArea->GetComponentLocation();
			}
			FRotator SpawnRotator = FRotator(0.f, 0.f, 0.f);

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
						OutputPlayerStart.RemoveAt(i);
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

