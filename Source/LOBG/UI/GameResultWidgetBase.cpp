// Fill out your copyright notice in the Description page of Project Settings.


#include "GameResultWidgetBase.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/Image.h"
#include "Blueprint/WidgetLayoutLibrary.h"

#include "MVPSlot.h"
#include "../Battle/BattleGS.h"
#include "../Battle/BattleGM.h"
#include "Kismet/GameplayStatics.h"

void UGameResultWidgetBase::NativeConstruct() {
	Super::NativeConstruct();
	ResultTitle = Cast<UTextBlock>(GetWidgetFromName(TEXT("ResultTitle")));
	MVPScrollBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("MVPScrollBox")));
	BlueMVP = Cast<UScrollBox>(GetWidgetFromName(TEXT("BlueMVP")));
	RedMVP = Cast<UScrollBox>(GetWidgetFromName(TEXT("RedMVP")));
	ExitButton = Cast<UButton>(GetWidgetFromName(TEXT("ExitButton")));

	if (ExitButton) {
		ExitButton->OnClicked.AddDynamic(this, &UGameResultWidgetBase::ExitBattle);
	}
}

void UGameResultWidgetBase::ExitBattle() {
	ABattleGM* GM = Cast<ABattleGM>(UGameplayStatics::GetGameMode(GetWorld()));
	GM->GoLobby();
}

void UGameResultWidgetBase::SetMVP() {
	ABattleGS* GS = Cast<ABattleGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (GS && MVPScrollBox) {
		TArray<FPlayerData> PlayerDataArr = GS->RedTabDataArray += GS->BlueTabDataArray;

		// 레벨 내림차순 정렬
		PlayerDataArr.Sort(
			[](const FPlayerData& A, const FPlayerData& B) {
				return A.PlayerLevel > B.PlayerLevel;
			}
		);

		bool BlueWin = true;
		for (int i = 0; MVPScrollBox->GetChildrenCount(); i++)
		{
			UMVPSlot* Child = Cast<UMVPSlot>(MVPScrollBox->GetChildAt(i));

			// 레벨
			Child->UserLVText->SetText(FText::FromString(""));

			// 이름
			Child->UserIDText->SetText(FText::FromString(PlayerDataArr[i].PlayerName));

			// 팀 컬러 아이콘
			if(PlayerDataArr[i].PlayerTeamColor == ETeamColor::Blue && BlueMVP){
				ResultTitle->SetText(FText::FromString(BlueWin ? "= WIN =": "- LOSE -"));
				Child->MVPIcon->SetBrush(Cast<UImage>(BlueMVP->GetChildAt(i))->Brush);
			} else if(PlayerDataArr[i].PlayerTeamColor == ETeamColor::Red && RedMVP){
				ResultTitle->SetText(FText::FromString(BlueWin ? "- LOSE -" : "- WIN -"));
				Child->MVPIcon->SetBrush(Cast<UImage>(RedMVP->GetChildAt(i))->Brush);
			}
		}
	}
}
