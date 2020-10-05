// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePC.h"
#include "BattleCharacter.h"
#include "BattleWidgetBase.h"
#include "../LOBGGameInstance.h"
#include "BattlePS.h"

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
		if (BattleWidgetClass)
		{
			BattleWidgetObject = CreateWidget<UBattleWidgetBase>(this, BattleWidgetClass);
			if (BattleWidgetObject)
			{
				BattleWidgetObject->AddToViewport();
			}
			SetInputMode(FInputModeGameOnly());
		}
		ULOBGGameInstance* GI = GetGameInstance<ULOBGGameInstance>();
		if (GI)
		{
			Server_SetPSTeamColor(GI->TeamColor);
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

void ABattlePC::Server_SetPSTeamColor_Implementation(const ETeamColor& TeamColor)
{
	ABattlePS* PS = GetPlayerState<ABattlePS>();
	if (PS)
	{
		ULOBGGameInstance* GI = GetGameInstance<ULOBGGameInstance>();
		if (GI)
		{
			PS->TeamColor = TeamColor;

			ABattleCharacter* PlayerPawn = Cast<ABattleCharacter>(GetPawn());
			if(PlayerPawn)
			{
				if (PS->TeamColor == ETeamColor::Red)
				{
					PlayerPawn->NetMulticast_AddTag(TEXT("Red"));
				}
				else if (PS->TeamColor == ETeamColor::Blue)
				{
					PlayerPawn->NetMulticast_AddTag(TEXT("Blue"));
				}
			}
		}
	}
}
