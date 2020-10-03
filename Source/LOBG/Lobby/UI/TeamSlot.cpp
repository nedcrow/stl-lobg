// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamSlot.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "LobbyWidgetBase.h"
#include "../LobbyGS.h"
#include "Kismet/GameplayStatics.h"

void UTeamSlot::NativeConstruct()
{
	Super::NativeConstruct();

	TeamColor = Cast<UBorder>(GetWidgetFromName(TEXT("TeamColor")));
	UserName = Cast<UTextBlock>(GetWidgetFromName(TEXT("UserName")));
}

void UTeamSlot::SetColor(FLinearColor NewColor)
{
	TeamColor->SetBrushColor(NewColor);
}

void UTeamSlot::SetUserName(FString NewName)
{
	UserName->SetText(FText::FromString(NewName));
}
