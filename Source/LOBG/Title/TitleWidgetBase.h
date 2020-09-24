// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API UTitleWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	class UEditableTextBox* ServerIDText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	class UEditableTextBox* UserIDText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	class UEditableTextBox* UserPasswardText;


	//Lobby에 입장
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UButton* ConnectServerButton;

	UFUNCTION()
		void ConnectServer();


	//서버 생성
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UButton* CreateServerButton;

	UFUNCTION()
		void StartServer();

	void SaveUserID();
};
