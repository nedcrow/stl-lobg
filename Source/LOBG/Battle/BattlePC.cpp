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
					UE_LOG(LogClass, Warning, TEXT("In PC UI Color is Red"));
					PlayerPawn->NetMulticast_AddTag(TEXT("Red"));
					//PlayerPawn->InitHPBar();
					//PlayerPawn->UIColor = FLinearColor(1, 0, 0, 1);
					//PlayerPawn->OnRep_SetUIColor();
				}
				else if (PS->TeamColor == ETeamColor::Blue)
				{
					UE_LOG(LogClass, Warning, TEXT("In PC UI Color is Blue"));
					PlayerPawn->NetMulticast_AddTag(TEXT("Blue"));
					//PlayerPawn->InitHPBar();
					//PlayerPawn->UIColor = FLinearColor(0, 0, 1, 1);
					//PlayerPawn->OnRep_SetUIColor();
				}
			}
		}
	}
}

void ABattlePC::TestUIColor()
{
	ABattlePS* PS = GetPlayerState<ABattlePS>();
	if (PS)
	{
		ULOBGGameInstance* GI = GetGameInstance<ULOBGGameInstance>();
		if (GI)
		{
			//PS->TeamColor = TeamColor;

			ABattleCharacter* PlayerPawn = Cast<ABattleCharacter>(GetPawn());
			if (PlayerPawn)
			{
				if (PS->TeamColor == ETeamColor::Red)
				{
					UE_LOG(LogClass, Warning, TEXT("In PC UI Color is Red"));
					PlayerPawn->NetMulticast_AddTag(TEXT("Red"));
					//PlayerPawn->InitHPBar();
					//PlayerPawn->UIColor = FLinearColor(1, 0, 0, 1);
					//PlayerPawn->OnRep_SetUIColor();
				}
				else if (PS->TeamColor == ETeamColor::Blue)
				{
					UE_LOG(LogClass, Warning, TEXT("In PC UI Color is Blue"));
					PlayerPawn->NetMulticast_AddTag(TEXT("Blue"));
					//PlayerPawn->InitHPBar();
					//PlayerPawn->UIColor = FLinearColor(0, 0, 1, 1);
					//PlayerPawn->OnRep_SetUIColor();
				}
			}
		}
	}
}
