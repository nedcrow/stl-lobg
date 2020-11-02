// Fill out your copyright notice in the Description page of Project Settings.


#include "ChattingWidgetBase.h"
#include "../LobbyPC.h"
#include "../../Battle/BattlePC.h"
#include "../../LOBGGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

void UChattingWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	ChatInput = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("ChatInput")));
	ChatScrollBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("ChatScrollBox")));
	ChatBG = Cast<UBorder>(GetWidgetFromName(TEXT("ChatBG")));

	if (ChatInput) {
		ChatInput->OnTextCommitted.AddDynamic(this, &UChattingWidgetBase::ProcessTextCommited);
		TArray<FString> TempArr;
		FString LevelName = GetWorld()->GetName();
		LevelName.ParseIntoArray(TempArr, TEXT("_"), true);
		if (TempArr[0] == "Step03") {
			ChatBG->SetBrushColor(FLinearColor(0, 0, 0, 0));
			ChatInput->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}



void UChattingWidgetBase::ProcessTextCommited(const FText& Text, ETextCommit::Type Method)
{
	switch (Method)
	{
	case ETextCommit::OnEnter:
	{

		TArray<FString> TempArr;
		FString LevelName = GetWorld()->GetName();
		LevelName.ParseIntoArray(TempArr, TEXT("_"), true);
		if (TempArr[0] == "Step02") {
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
		else if (TempArr[0] == "Step03") {
			ABattlePC* PC = GetOwningPlayer<ABattlePC>();
			if (PC) {
				ULOBGGameInstance* GI = Cast<ULOBGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
				if (GI) {
					if (ChatInput->Text.ToString() == "") {
						ExitInputBox(PC);
					}
					else {
						FString Temp = FString::Printf(TEXT("%s : %s"), *GI->GetUserID(), *Text.ToString());
						PC->Server_SendMessageInBattle(FText::FromString(Temp));
						ChatInput->SetText(FText::FromString(TEXT("")));
					}
				}
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
			APlayerController* PC = GetOwningPlayer<APlayerController>();

			FTimerHandle EraseHandle;
			PC->GetWorldTimerManager().SetTimer(EraseHandle, this, &UChattingWidgetBase::EraseMessage, 3.f, false);
		}
	}
}

void UChattingWidgetBase::EraseMessage() {
	if (ChatInput->Visibility == ESlateVisibility::Collapsed) {
		ChatScrollBox->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UChattingWidgetBase::ExitInputBox(APlayerController * PC)
{
	SetUserFocus(PC);
	ChatInput->SetVisibility(ESlateVisibility::Collapsed);
	ChatBG->SetBrushColor(FLinearColor(0, 0, 0, 0));
	PC->SetInputMode(FInputModeGameOnly());

	FTimerHandle EraseHandle;
	PC->GetWorldTimerManager().SetTimer(EraseHandle, this, &UChattingWidgetBase::EraseMessage, 3.f, false);
}
