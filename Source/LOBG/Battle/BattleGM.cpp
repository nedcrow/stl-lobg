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
		for (int i = 0; i < 6; i++)
		{
			AAIManager * AIM = GetWorld()->SpawnActor<AAIManager>(AIManagerClass);
			if (AIM)
			{
				AIManagers.Emplace(AIM);
				int index = i%2;
				if (index == 0) {
					AIM->TeamName = TEXT("Red");
				}
				else {
					AIM->TeamName = TEXT("Blue");
					AIM->ReverseCoursePoints(AIM->CoursePoints); // 이동 순서 뒤집기.
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

// AI매니저
AAIManager* ABattleGM::FindAIM(AAIController * AIPawnOwner)
{
	if (AIPawnOwner && AIManagers.Num() > 0)
	{
		APawn* AIPawn = AIPawnOwner->GetPawn();
		if (AIPawn)
		{
			for (int i = 0; i < AIManagers.Num(); i++)
			{
				if (AIPawn->ActorHasTag(AIManagers[i]->TeamName))		// AI컨트롤러의 팀과 같은 팀의 AI매니저 찾기.
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
			//리스폰 위치 가져오기
			FVector SpawnLocation = FVector(0.f, 0.f, 1000.f);

			TArray<AActor*> RedReSpawnArray;
			UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AReSpawn::StaticClass(), TEXT("Red"), RedReSpawnArray);

			TArray<AActor*> BlueReSpawnArray;
			UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AReSpawn::StaticClass(), TEXT("Blue"), BlueReSpawnArray);

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
							SpawnLocation = ReSpawnObj->ReSpawnArea->GetComponentLocation();
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
							SpawnLocation = ReSpawnObj->ReSpawnArea->GetComponentLocation();
							break;
						}
					}
				}
			}
			FRotator SpawnRotator = FRotator(0.f, 0.f, 0.f);

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
	//플레이어의 메시설정
	CreatePlayerMeshWidget();
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
					OutputPlayerStart.RemoveAt(i);
					break;
				}
				//개발용으로 팀을 설정하지 않을때를 위한 else코드
				//나중에 지워야함
				else if (TestMapVersonSpawn)
				{
					Controller->Client_TestWidget();
					//Controller->MeshWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
					ABattleCharacter* PlayerPawn = GetWorld()->SpawnActor<ABattleCharacter>(
						PlayerClass, OutputPlayerStart[i]->GetActorLocation(), OutputPlayerStart[i]->GetActorRotation());
					Controller->Possess(PlayerPawn);
					Controller->InitPlayerWithTeam();
					OutputPlayerStart.RemoveAt(i);
					break;
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

