// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePC.h"
#include "BattleCharacter.h"
#include "BattleWidgetBase.h"
#include "../LOBGGameInstance.h"
#include "BattlePS.h"
#include "BattleGM.h"
#include "Kismet/GameplayStatics.h"

void ABattlePC::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &ABattlePC::ClickFire);
	InputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Released, this, &ABattlePC::ReleaseFire);
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
