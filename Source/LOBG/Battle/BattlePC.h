// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../LOBGGameInstance.h"
#include "../ChoiceMesh/MeshWidgetBase.h"
#include "BattlePC.generated.h"

/**
 *
 */
UCLASS()
class LOBG_API ABattlePC : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void SetupInputComponent()override;
	virtual void BeginPlay() override;

	//마우스 왼쪽 버튼으로 실행되는 발사함수
	void ClickFire();
	void ReleaseFire();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		TSubclassOf<class UBattleWidgetBase> BattleWidgetClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UBattleWidgetBase* BattleWidgetObject;

	void InitPlayerWithTeam();

	UFUNCTION(Client, Reliable)
		void Client_TestWidget();
	void Client_TestWidget_Implementation();

	UFUNCTION(Server, Reliable)
		void Server_SetMyUserName(const FString& MyName);
	void Server_SetMyUserName_Implementation(const FString& MyName);

	FString MyUserName;

	void PushOpenStore();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		TSubclassOf<class UStoreWidgetBase> StoreWidgetClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UStoreWidgetBase* StoreWidgetObject;

	bool bStoreOpen = false;

	UFUNCTION(Client, Reliable)
		void Client_CreateMeshWidget();
	void Client_CreateMeshWidget_Implementation();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		TSubclassOf<class UMeshWidgetBase> MeshWidgetClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UMeshWidgetBase* MeshWidgetObject;

	void UpdateGameStartTime();

	UFUNCTION(Server, Reliable)
		void Server_MakePlayerInGM(const EMeshType& MyMeshType);
	void Server_MakePlayerInGM_Implementation(const EMeshType& MyMeshType);

	UFUNCTION(Client, Reliable)
		void Client_SetGameStartUI(const int& StartTime);
	void Client_SetGameStartUI_Implementation(const int& StartTime);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		TSubclassOf<class UGameStartWidgetBase> GameStartWidgetClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UGameStartWidgetBase* GameStartWidgetObject;

	void UpdateGSTabArrayData();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		TSubclassOf<class UTabWidgetBase> TabWidgetClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		class UTabWidgetBase* TabWidgetObject;

	void OpenTab();
	void CloseTab();

	void InitTabPlayer();

	// Result
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	TSubclassOf<class UGameResultWidgetBase> ResultWidgetClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	class UGameResultWidgetBase* ResultWidgetObject;

	UFUNCTION(Client, Reliable)
	void Client_OpenResultTab(ETeamColor WinColor);
	void Client_OpenResultTab_Implementation(ETeamColor WinColor);

	UFUNCTION(Client, Reliable)
		void Client_AddPotionSlot(int DataNumber);
	void Client_AddPotionSlot_Implementation(int DataNumber);


	// Chatting
	uint8 bIsChattingMode:1;
	void OnFocusChattingWidget();
	void OffGameCursor();

	UFUNCTION(Server, Reliable)
	void Server_SendMessageInBattle(const FText& Message);
	void Server_SendMessageInBattle_Implementation(const FText& Message);

	UFUNCTION(Client, Reliable)
	void Client_SendMessageInBattle(const FText& Message);
	void Client_SendMessageInBattle_Implementation(const FText& Message);

	UFUNCTION(Client, Reliable)
		void Client_InitCurrentGunName();
		void Client_InitCurrentGunName_Implementation();
};