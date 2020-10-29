// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePC.h"
#include "BattleCharacter.h"
#include "BattleWidgetBase.h"
#include "../LOBGGameInstance.h"
#include "BattlePS.h"
#include "BattleGM.h"
#include "BattleGS.h"
#include "Kismet/GameplayStatics.h"
#include "../Store/StoreWidgetBase.h"
#include "../Store/StoreItemWidgetBase.h"
#include "../ChoiceMesh/MeshWidgetBase.h"
#include "../UI/GameStartWidgetBase.h"
#include "../UI/TabWidgetBase.h"
#include "../UI/GameResultWidgetBase.h"

void ABattlePC::SetupInputComponent()
{
	Super::SetupInputComponent();
	//InputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &ABattlePC::ClickFire);
	//InputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Released, this, &ABattlePC::ReleaseFire);

	InputComponent->BindAction(TEXT("OpenStore"), EInputEvent::IE_Pressed, this, &ABattlePC::PushOpenStore);
	InputComponent->BindAction(TEXT("Tab"), EInputEvent::IE_Pressed, this, &ABattlePC::OpenTab);
	InputComponent->BindAction(TEXT("Tab"), EInputEvent::IE_Released, this, &ABattlePC::CloseTab);
}

void ABattlePC::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		ULOBGGameInstance* GI = GetGameInstance<ULOBGGameInstance>();
		if (GI)
		{
			//서버에 아이디 보내주기
			Server_SetMyUserName(GI->GetUserID());
		}
	}
}

// 사격은 캐릭터에서
void ABattlePC::ClickFire()
{
	ABattleCharacter* BattlePlayer = GetPawn<ABattleCharacter>();
	if (BattlePlayer)
	{
		BattlePlayer->StartFire();
	}

}

void ABattlePC::ReleaseFire()
{
	ABattleCharacter* BattlePlayer = GetPawn<ABattleCharacter>();
	if (BattlePlayer)
	{
		BattlePlayer->StopFire();
	}
}

void ABattlePC::InitPlayerWithTeam()
{
	ABattlePS* PS = GetPlayerState<ABattlePS>();
	if (PS)
	{
		ABattleCharacter* PlayerPawn = Cast<ABattleCharacter>(GetPawn());
		if (PlayerPawn)
		{
			ABattleGS* GS = Cast<ABattleGS>(UGameplayStatics::GetGameState(GetWorld()));
			if (GS)
			{
				if (PS->TeamColor == ETeamColor::Red)
				{
					PlayerPawn->NetMulticast_AddTag(TEXT("Red"));
				}
				else if (PS->TeamColor == ETeamColor::Blue)
				{
					PlayerPawn->NetMulticast_AddTag(TEXT("Blue"));
				}
				PlayerPawn->NetMulticast_InitHPBar(PS->TeamColor);
				PlayerPawn->NetMulticast_SetMeshSettings(PS->PlayerMeshType);
			}
		}
	}
}

void ABattlePC::Client_TestWidget_Implementation()
{
	if (IsLocalPlayerController())
	{
		if (BattleWidgetClass)
		{
			BattleWidgetObject = CreateWidget<UBattleWidgetBase>(this, BattleWidgetClass);
			if (BattleWidgetObject)
			{
				BattleWidgetObject->AddToViewport();
			}
			SetInputMode(FInputModeGameOnly());
			bShowMouseCursor = false;
		}

		if (StoreWidgetClass)
		{
			StoreWidgetObject = CreateWidget<UStoreWidgetBase>(this, StoreWidgetClass);
			if (StoreWidgetObject)
			{
				StoreWidgetObject->AddToViewport();
				StoreWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
			}
		}

		if (TabWidgetClass)
		{
			TabWidgetObject = CreateWidget<UTabWidgetBase>(this, TabWidgetClass);
			if (TabWidgetObject)
			{
				TabWidgetObject->AddToViewport();
				TabWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
			}
		}

		if (MeshWidgetClass)
		{
			if (MeshWidgetObject)
			{
				MeshWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
			}
		}

		if (ResultWidgetClass)
		{
			ResultWidgetObject = CreateWidget<UGameResultWidgetBase>(this, ResultWidgetClass);
			if (ResultWidgetObject)
			{
				ResultWidgetObject->AddToViewport();
				ResultWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void ABattlePC::Server_SetMyUserName_Implementation(const FString& MyName)
{
	MyUserName = MyName;

	ABattlePS* PS = GetPlayerState<ABattlePS>();
	if (PS)
	{
		PS->MyPlayerData.PlayerName = MyName;
	}

	ABattleGM* GM = Cast<ABattleGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		//서버의 PC들이 아이디를 다 가지고 있는지 검사하는 함수 실행
		GM->CheckAllControllerHasName();
	}
}

void ABattlePC::PushOpenStore()
{
	if (IsLocalPlayerController() && bStoreOpen)
	{
		//범위 안에서 보여져 있다면 안보이게 만들고 return
		if (StoreWidgetObject->GetVisibility() == ESlateVisibility::Visible)
		{
			StoreWidgetObject->SetVisiBilitySlot(ESlateVisibility::Collapsed);
			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
			return;
		}
		//숨겨져 있는 상태라면 보이게 만든다
		StoreWidgetObject->SetVisiBilitySlot(ESlateVisibility::Visible);
		

		bShowMouseCursor = true;
		SetInputMode(FInputModeGameAndUI());
	}
	//범위밖에서 누르면 안보이게 한다 범위 밖에서는 아무리 눌러도 보이지 않는다
	else if (!bStoreOpen)
	{
		StoreWidgetObject->SetVisiBilitySlot(ESlateVisibility::Collapsed);
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
	}
}

void ABattlePC::Client_CreateMeshWidget_Implementation()
{
	if (IsLocalPlayerController())
	{
		if (MeshWidgetClass)
		{
			MeshWidgetObject = CreateWidget<UMeshWidgetBase>(this, MeshWidgetClass);
			if (MeshWidgetObject)
			{
				MeshWidgetObject->AddToViewport();
				SetInputMode(FInputModeUIOnly());
				bShowMouseCursor = true;
			}
		}

		if (GameStartWidgetClass)
		{
			GameStartWidgetObject = CreateWidget<UGameStartWidgetBase>(this, GameStartWidgetClass);
			if (GameStartWidgetObject)
			{
				GameStartWidgetObject->AddToViewport();
				if (HasAuthority())
				{
					ABattleGS* GS = Cast<ABattleGS>(UGameplayStatics::GetGameState(GetWorld()));
					if (GS)
					{
						GS->GameStartTime = 10;
						GS->OnRep_GameStartTime();
						FTimerHandle StartTimerHandle;
						GetWorldTimerManager().SetTimer(StartTimerHandle, this, &ABattlePC::UpdateGameStartTime, 1.f, false);
					}
				}
			}
		}
	}
}

void ABattlePC::UpdateGameStartTime()
{
	ABattleGS* GS = Cast<ABattleGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (GS)
	{
		GS->GameStartTime -= 1;
		GS->OnRep_GameStartTime();
		if (GS->GameStartTime == 0)
		{
			ABattleGM* GM = Cast<ABattleGM>(UGameplayStatics::GetGameMode(GetWorld()));
			if (GM)
			{
				GM->StartAIMinion();
			}
		}
		else
		{
			FTimerHandle StartTimerHandle;
			GetWorldTimerManager().SetTimer(StartTimerHandle, this, &ABattlePC::UpdateGameStartTime, 1.f, false);
		}
	}
}

void ABattlePC::Server_MakePlayerInGM_Implementation(const EMeshType& MyMeshType)
{
	ABattlePS* PS = GetPlayerState<ABattlePS>();
	if (PS)
	{
		PS->MyPlayerData.PlayerMesh = MyMeshType;
	}
	ABattleGM* GM = Cast<ABattleGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		GM->PlayerSpawn_Test(this, MyMeshType);
	}
}

void ABattlePC::Client_SetGameStartUI_Implementation(const int & StartTime)
{
	int GameStartTimeInPC = StartTime;
	GameStartWidgetObject->SetGameStartTimeText(GameStartTimeInPC);
}

void ABattlePC::UpdateGSTabArrayData()
{
	ABattleGS* GS = Cast<ABattleGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (GS)
	{
		ABattlePS* PS = GetPlayerState<ABattlePS>();
		if (PS)
		{
			if (PS->TeamColor == ETeamColor::Red)
			{
				for (int i = 0; i < GS->RedTabDataArray.Num(); ++i)
				{
					if (GS->RedTabDataArray[i].PlayerName == PS->MyPlayerData.PlayerName)
					{
						GS->RedTabDataArray[i] = PS->MyPlayerData;
					}
				}
			}
			else if(PS->TeamColor == ETeamColor::Blue)
			{
				for (int i = 0; i < GS->BlueTabDataArray.Num(); ++i)
				{
					if (GS->BlueTabDataArray[i].PlayerName == PS->MyPlayerData.PlayerName)
					{
						GS->BlueTabDataArray[i] = PS->MyPlayerData;
					}
				}
			}
		}
	}
}

void ABattlePC::OpenTab()
{
	if (TabWidgetObject)
	{
		TabWidgetObject->UpdateSlot();
		TabWidgetObject->SetVisibility(ESlateVisibility::Visible);
	}
}

void ABattlePC::CloseTab()
{
	if (TabWidgetObject)
	{
		TabWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ABattlePC::InitTabPlayer()
{
	ABattlePS* PS = GetPlayerState<ABattlePS>();
	if (PS)
	{
		ABattleGS* GS = Cast<ABattleGS>(UGameplayStatics::GetGameState(GetWorld()));
		if (GS)
		{
			if (PS->TeamColor == ETeamColor::Red)
			{
				GS->RedTabDataArray.Add(PS->MyPlayerData);
			}
			else if (PS->TeamColor == ETeamColor::Blue)
			{
				GS->BlueTabDataArray.Add(PS->MyPlayerData);
			}
		}
	}
}

void ABattlePC::Client_OpenResultTab_Implementation(ETeamColor WinColor)
{
	UE_LOG(LogTemp,Warning,TEXT("SetMVP"));
	if (ResultWidgetObject) {
		ResultWidgetObject->SetMVP(WinColor);
		ResultWidgetObject->SetVisibility(ESlateVisibility::Visible);
		bShowMouseCursor = true;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Havent ResultWidgetObject"));
	}
}
