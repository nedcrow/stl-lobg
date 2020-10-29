// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGM.h"
#include "BattleCharacter.h"
#include "../ReSpawn/ReSpawn.h"
#include "Kismet/GameplayStatics.h"
#include "BattlePC.h"
#include "BattlePS.h"
#include "BattleGS.h"
#include "../AICharacter/AIManager.h"
#include "../AICharacter/AIMinionChar.h"
#include "../Temp/TempTower.h"
#include "../LOBGGameInstance.h"
#include "GameFramework/PlayerStart.h"
#include "AIController.h"
#include "../AICharacter/Fairy/FairyPawn.h"

void ABattleGM::BeginPlay()
{
	Super::BeginPlay();
	ULOBGGameInstance* GI = GetGameInstance<ULOBGGameInstance>();
	if (GI)
	{
		TeamRedUsers = GI->TeamRedUsers;
		TeamBlueUsers = GI->TeamBlueUsers;

		//������ ���� �ڵ� ���� �������� �ʰ� Test �ʿ��� ����� �� ĳ���͸� �����ϱ� ����
		//���߿� ���� ����
		if (TeamRedUsers.Num() + TeamBlueUsers.Num() == 0)
		{
			TestMapVersonSpawn = true;
		}
	}
	FindPlayerStart();

	FindTowers();
}

void ABattleGM::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

// AI�Ŵ���
AAIManager* ABattleGM::FindAIM(AAIController * AIPawnOwner)
{
	if (AIPawnOwner && AIManagers.Num() > 0)
	{
		AAIMinionChar* AIPawn = AIPawnOwner->GetPawn<AAIMinionChar>();
		if (AIPawn)
		{
			for (int i = 0; i < AIManagers.Num(); i++)
			{
				//if (AIPawn->ActorHasTag(AIManagers[i]->TeamName))		// AI��Ʈ�ѷ��� ���� ���� ���� AI�Ŵ��� ã��.
				//{
				//	return AIManagers[i];
				//}

				if (AIPawn->ActorHasTag(AIManagers[i]->TeamName) && AIManagers[i]->WaveCourse == AIPawn->WaveCourse)		// AI��Ʈ�ѷ��� ���� ���� ���� AI�Ŵ��� ã��. 6 AIManager Version.
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
			FRotator SpawnRotator = FRotator(0, 0, 0);

			TArray<AActor*> RedReSpawnArray;
			UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AReSpawn::StaticClass(), TEXT("Red"), RedReSpawnArray);

			TArray<AActor*> BlueReSpawnArray;
			UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AReSpawn::StaticClass(), TEXT("Blue"), BlueReSpawnArray);

			TArray<AActor*> NoneReSpawnArray;
			UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AReSpawn::StaticClass(), TEXT("None"), NoneReSpawnArray);

			ABattlePS* PS = Cast<ABattlePS>(Pawn->GetPlayerState());
			if (PS)
			{
				if (PS->TeamColor == ETeamColor::Red)
				{
					for (int i = 0; i < RedReSpawnArray.Num(); ++i)
					{
						AReSpawn* ReSpawnObj = Cast<AReSpawn>(RedReSpawnArray[i]);
						if (ReSpawnObj)
						{
							//SpawnLocation = ReSpawnObj->ReSpawnArea->GetComponentLocation();
							SpawnLocation = ReSpawnObj->ReSpawnArea->GetComponentToWorld().GetLocation();
							//SpawnRotator = ReSpawnObj->GetActorRotation();
							break;
						}
					}
				}
				else if (PS->TeamColor == ETeamColor::Blue)
				{
					for (int i = 0; i < BlueReSpawnArray.Num(); ++i)
					{
						AReSpawn* ReSpawnObj = Cast<AReSpawn>(BlueReSpawnArray[i]);
						if (ReSpawnObj)
						{
							//SpawnLocation = ReSpawnObj->ReSpawnArea->GetComponentLocation();
							SpawnLocation = ReSpawnObj->ReSpawnArea->GetComponentToWorld().GetLocation();
							//SpawnRotator = ReSpawnObj->GetActorRotation();
							break;
						}
					}
				}

				else if (PS->TeamColor == ETeamColor::None)
				{
					for (int i = 0; i < NoneReSpawnArray.Num(); ++i)
					{
						AReSpawn* ReSpawnObj = Cast<AReSpawn>(NoneReSpawnArray[i]);
						if (ReSpawnObj)
						{
							SpawnLocation = ReSpawnObj->ReSpawnArea->GetComponentToWorld().GetLocation();
							SpawnRotator = ReSpawnObj->GetActorRotation();
							break;
						}
					}
				}
			}
			

			ABattleCharacter* BattlePlayer = GetWorld()->SpawnActor<ABattleCharacter>(PlayerClass, SpawnLocation, SpawnRotator);

			Pawn->Destroy();

			//�÷��̾���Ʈ�ѷ��� ����
			PC->Possess(BattlePlayer);

			//��Ʈ�ѷ� ���� �� �ʱ�ȭ�� ��ϵ�
			BattlePlayer->NetMulticast_ReSpawnUI();
			PC->InitPlayerWithTeam();
		}
	}
}

void ABattleGM::FindTowers()
{
	TArray<AActor*> RedOutTowers;
	TArray<AActor*> BlueOutTowers;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), FairyTowerClass, TEXT("Red"), RedOutTowers);
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), FairyTowerClass, TEXT("Blue"), BlueOutTowers);
	//RedTowerCount = RedOutTowers.Num();
	//BlueTowerCount = BlueOutTowers.Num();
}

void ABattleGM::CountTower(ETeamColor DestroyTowerColor)
{
	switch (DestroyTowerColor)
	{
	case ETeamColor::None:
		break;
	case ETeamColor::Red:
		RedTowerCount--;
		break;
	case ETeamColor::Blue:
		BlueTowerCount--;
		break;
	default:
		break;
	}

	if (RedTowerCount == 0)
	{
		FTimerHandle GameOverTimer;
		GetWorldTimerManager().SetTimer(GameOverTimer, this, &ABattleGM::GoLobby, 3.f);
	}
	else if (BlueTowerCount == 0)
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
						PS->MyPlayerData.PlayerTeamColor = ETeamColor::Red;
						
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
						PS->MyPlayerData.PlayerTeamColor = ETeamColor::Blue;
						break;
					}
				}
			}
		}
	}
	//�÷��̾��� �޽ü���
	CreatePlayerMeshWidget();
}

//���� �Ⱦ��� �Լ� Ȥ�� ���� ���ܵ�
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

void ABattleGM::PlayerSpawn_Test(ABattlePC* Controller, EMeshType MyMeshType)
{
	if (Controller)
	{
		ABattlePS* PS = Controller->GetPlayerState<ABattlePS>();
		if (PS)
		{
			PS->PlayerMeshType = MyMeshType;

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

					Controller->Client_TestWidget();
					ABattleCharacter* PlayerPawn = GetWorld()->SpawnActor<ABattleCharacter>(
						PlayerClass, OutputPlayerStart[i]->GetActorLocation(), OutputPlayerStart[i]->GetActorRotation());
					Controller->Possess(PlayerPawn);
					Controller->InitPlayerWithTeam();
					Controller->InitTabPlayer();
					OutputPlayerStart.RemoveAt(i);
					break;
				}
				//���߿����� ���� �������� �������� ���� else�ڵ�
				//���߿� ��������
				else if (TestMapVersonSpawn)
				{
					Controller->Client_TestWidget();
					//Controller->MeshWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
					ABattleCharacter* PlayerPawn = GetWorld()->SpawnActor<ABattleCharacter>(
						PlayerClass, OutputPlayerStart[i]->GetActorLocation(), OutputPlayerStart[i]->GetActorRotation());
					Controller->Possess(PlayerPawn);
					Controller->InitPlayerWithTeam();
					Controller->InitTabPlayer();
					OutputPlayerStart.RemoveAt(i);
					break;
				}
			}
		}
	}
}

void ABattleGM::CheckAllControllerHasName()
{
	//���߿� PC�� ���� �˻��ؼ� �̸��� ����Ǿ��ִٸ����� ��������

	CheckControllerHasName.Add(true);
	int AllPlayerNum = TeamRedUsers.Num() + TeamBlueUsers.Num();
	if (CheckControllerHasName.Num() == AllPlayerNum || TestMapVersonSpawn)
	{
		//������ PC���� ���̵� �� ������ �ִٸ� PS�� �� ����
		SetPSTeamColor();
	}
}

void ABattleGM::CreatePlayerMeshWidget()
{
	for (auto Iter = GetWorld()->GetControllerIterator(); Iter; ++Iter)
	{
		ABattlePC* PC = Cast<ABattlePC>(*Iter);
		if (PC)
		{
			PC->Client_CreateMeshWidget();
		}
	}
}

void ABattleGM::SetGameStartTimeWidget(int GameTime)
{
	for (auto Iter = GetWorld()->GetControllerIterator(); Iter; ++Iter)
	{
		ABattlePC* PC = Cast<ABattlePC>(*Iter);
		if (PC)
		{
			PC->Client_SetGameStartUI(GameTime);
		}
	}
}

void ABattleGM::StartAIMinion()
{
	if (AIManagerClass)
	{
		for (int i = 0; i < 6; i++)
		{
			AAIManager * AIM = GetWorld()->SpawnActor<AAIManager>(AIManagerClass);
			if (AIM)
			{
				AIManagers.Emplace(AIM);
				int index = i % 2;
				if (index == 0)
				{
					AIM->TeamName = TEXT("Red");
				}
				else
				{
					AIM->TeamName = TEXT("Blue");
				}
				AIM->WaveCourse = FMath::FloorToInt(i / 2) + 1;	// Manager ���� WaveCourse �Ҵ�
				AIM->SeachCoursePoints2();
			}
		}

		if (SpawnAINumber > 0)
		{
			RepeatMinionsWave();
		}
	}
}

void ABattleGM::CallOpenResultTab(ETeamColor WinColor)
{
	for (auto Iter = GetWorld()->GetControllerIterator(); Iter; ++Iter)
	{
		ABattlePC* PC = Cast<ABattlePC>(*Iter);
		if (PC)
		{
			PC->Client_OpenResultTab(WinColor);
		}
	}
}

