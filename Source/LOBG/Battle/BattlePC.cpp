// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePC.h"
#include "BattleCharacter.h"
#include "BattleWidgetBase.h"
#include "../LOBGGameInstance.h"
#include "BattlePS.h"
#include "BattleGM.h"
#include "Kismet/GameplayStatics.h"
#include "../Store/StoreWidgetBase.h"

void ABattlePC::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &ABattlePC::ClickFire);
	InputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Released, this, &ABattlePC::ReleaseFire);

	InputComponent->BindAction(TEXT("OpenStore"), EInputEvent::IE_Pressed, this, &ABattlePC::PushOpenStore);
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
			if (PS->TeamColor == ETeamColor::Red)
			{
				PlayerPawn->NetMulticast_AddTag(TEXT("Red"));
			}
			else if (PS->TeamColor == ETeamColor::Blue)
			{
				PlayerPawn->NetMulticast_AddTag(TEXT("Blue"));
			}
			PlayerPawn->NetMulticast_InitHPBar(PS->TeamColor);
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
	}
}

void ABattlePC::Server_SetMyUserName_Implementation(const FString& MyName)
{
	MyUserName = MyName;
	
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
			StoreWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
			return;
		}
		//숨겨져 있는 상태라면 보이게 만든다
		StoreWidgetObject->SetVisibility(ESlateVisibility::Visible);
		bShowMouseCursor = true;
		SetInputMode(FInputModeGameAndUI());
	}
	//범위밖에서 누르면 안보이게 한다 범위 밖에서는 아무리 눌러도 보이지 않는다
	else if (!bStoreOpen)
	{
		StoreWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
	}
}
