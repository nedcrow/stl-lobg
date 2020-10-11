// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamSlot.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Kismet/KismetInputLibrary.h"
#include "LobbyWidgetBase.h"
#include "../LobbyPC.h"
#include "MouseButtonSlot.h"

void UTeamSlot::NativeConstruct()
{
	Super::NativeConstruct();

	TeamColor = Cast<UBorder>(GetWidgetFromName(TEXT("TeamColor")));
	UserName = Cast<UTextBlock>(GetWidgetFromName(TEXT("UserName")));
	SlotButton = Cast<UButton>(GetWidgetFromName(TEXT("SlotButton")));

	SlotButton->OnHovered.AddDynamic(this, &UTeamSlot::HoveredSlotButton);
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
	bHovered = true;
	FPointerEvent SlotMouseEvent;
	
	ALobbyPC* PC = Cast<ALobbyPC>(GetOwningPlayer());
	if (PC)
	{
		//PC->LobbyWidgetObject->MouseButtonSlot->SetRenderTransform(SlotButton->GetRenderTransformAngle())
	}
	//마우스 오른쪽 버튼 클릭 시
	if (UKismetInputLibrary::PointerEvent_IsMouseButtonDown(SlotMouseEvent, EKeys::RightMouseButton))
	{
		ULobbyWidgetBase* LobbyWidget = Cast<ULobbyWidgetBase>(GetWidgetTreeOwningClass());
		if (LobbyWidget)
		{
			//LobbyWidget
		}
	}
}
