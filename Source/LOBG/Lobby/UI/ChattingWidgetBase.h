// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChattingWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API UChattingWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UEditableTextBox* ChatInput;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UScrollBox* ChatScrollBox;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UBorder* ChatBG;
	UFUNCTION()
		void ProcessTextCommited(const FText& Text, ETextCommit::Type Method);
		void AddMessage(FText Message);
		void EraseMessage();
		void ExitInputBox(APlayerController * PC);
};
