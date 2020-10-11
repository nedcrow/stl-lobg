// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamSlot.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Kismet/KismetInputLibrary.h"
#include "LobbyWidgetBase.h"
#include "../LobbyPC.h"
#include "MouseButtonSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"

void UTeamSlot::NativeConstruct()
{
	Super::NativeConstruct();

	TeamColor = Cast<UBorder>(GetWidgetFromName(TEXT("TeamColor")));
	UserName = Cast<UTextBlock>(GetWidgetFromName(TEXT("UserName")));
	SlotButton = Cast<UButton>(GetWidgetFromName(TEXT("SlotButton")));
	MouseButtonSlot = Cast<UMouseButtonSlot>(GetWidgetFromName(TEXT("MouseButtonSlot")));

	SlotButton->OnHovered.AddDynamic(this, &UTeamSlot::HoveredSlotButton);

	if (MouseButtonSlot)
	{
		MouseButtonSlot->SetVisibility(ESlateVisibility::Collapsed);
	}

	MouseButtonSlot->SlotButton->OnClicked.AddDynamic(this, &UTeamSlot::ClickedChangeTeam);
}

void UTeamSlot::SetColor(FLinearColor NewColor)
{
	TeamColor->SetBrushColor(NewColor);
}

void UTeamSlot::SetUserName(FString NewName)
{
	UserName->SetText(FText::FromString(NewName));
}

void UTeamSlot::HoveredSlotButton()
{
	FPointerEvent SlotMouseEvent;
	
	//마우스 오른쪽 버튼 클릭 시
	if (UKismetInputLibrary::PointerEvent_IsMouseButtonDown(SlotMouseEvent, EKeys::RightMouseButton))
	{
		MouseButtonSlot->SetVisibility(ESlateVisibility::Visible);
	}
}

void UTeamSlot::ClickedChangeTeam()
{
	FString SlotUserName = UserName->GetText().ToString();
	ALobbyPC* PC = Cast<ALobbyPC>(GetOwningPlayer());
	if (PC)
	{
		PC->Server_ChangeTeam(SlotUserName);
	}
	MouseButtonSlot->SetVisibility(ESlateVisibility::Collapsed);
}
