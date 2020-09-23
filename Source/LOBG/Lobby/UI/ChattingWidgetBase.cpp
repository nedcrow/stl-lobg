// Fill out your copyright notice in the Description page of Project Settings.


#include "ChattingWidgetBase.h"
#include "../LobbyPC.h"
#include "../../LOBGGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

void UChattingWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	ChatInput = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("ChatInput")));
	ChatScrollBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("ChatScrollBox")));

	if (ChatInput) {
		ChatInput->OnTextCommitted.AddDynamic(this, &UChattingWidgetBase::ProcessTextCommited);
	}
}



void UChattingWidgetBase::ProcessTextCommited(const FText& Text, ETextCommit::Type Method)
{
	switch (Method)
	{
	case ETextCommit::OnEnter:
	{
		ALobbyPC* PC = GetOwningPlayer<ALobbyPC>();
		if (PC) {
			ULOBGGameInstance* GI = Cast<ULOBGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
			if (GI) {
				FString Temp = FString::Printf(TEXT("%s : %s"), *GI->GetUserID(), *Text.ToString());
				PC->Server_SendMessage(FText::FromString(Temp));
				ChatInput->SetText(FText::FromString(TEXT("")));
			}
		}
	}
	break;
	case ETextCommit::OnCleared:
	{
		ChatInput->SetUserFocus(GetOwningPlayer());
	}
	break;
	}
}

void UChattingWidgetBase::AddMessage(FText Message)
{
	if (ChatScrollBox) {
		UTextBlock* TempTextBlock = NewObject<UTextBlock>(ChatScrollBox);
		if (TempTextBlock) {
			TempTextBlock->SetText(Message);
			ChatScrollBox->AddChild(TempTextBlock);
			ChatScrollBox->ScrollToEnd();
		}
	}
}