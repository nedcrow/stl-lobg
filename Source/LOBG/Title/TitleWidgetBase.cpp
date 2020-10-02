// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleWidgetBase.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "TitlePC.h"
#include "../LOBGGameInstance.h"

void UTitleWidgetBase::NativeConstruct()
{
	ConnectServerButton = Cast<UButton>(GetWidgetFromName(TEXT("ConnectServerButton")));
	CreateServerButton = Cast<UButton>(GetWidgetFromName(TEXT("CreateServerButton")));
	ServerIDText = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("ServerIDText")));
	UserIDText = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("IDBox")));
	UserPasswardText = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("PasswordBox")));

	if (ConnectServerButton)
	{
		ConnectServerButton->OnClicked.AddDynamic(this, &UTitleWidgetBase::ConnectServer);
	}

	if (CreateServerButton)
	{
		CreateServerButton->OnClicked.AddDynamic(this, &UTitleWidgetBase::StartServer);
	}
}

void UTitleWidgetBase::ConnectServer()
{
	ATitlePC* PC = GetOwningPlayer<ATitlePC>();
	if (PC)
	{
		SaveUserID();
		FString ServerAddress = ServerIDText->GetText().ToString();
		PC->ConnectServer(ServerAddress);
	}
}

void UTitleWidgetBase::StartServer()
{
	ATitlePC* PC = GetOwningPlayer<ATitlePC>();
	if (PC)
	{
		SaveUserID();
		PC->StartServer();
	}
}

void UTitleWidgetBase::SaveUserID()
{
	ULOBGGameInstance* GI = GetGameInstance<ULOBGGameInstance>();
	if (GI)
	{
		FString UserID = UserIDText->GetText().ToString();
		if (UserID.IsEmpty())
		{
			UserID = FString::Printf(TEXT("%dUser"), FMath::RandRange(1, 100));
		}
		GI->SetUserID(UserID);
	}
}
