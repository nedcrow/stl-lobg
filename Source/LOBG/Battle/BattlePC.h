// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../LOBGGameInstance.h"
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

	UFUNCTION(Server, Reliable)
		void Server_SetPSTeamColor(const ETeamColor& TeamColor);
	void Server_SetPSTeamColor_Implementation(const ETeamColor& TeamColor);

	void InitTeamColor();

	UFUNCTION(Client, Reliable)
		void Clinet_SetTeamColorInPC();
	void Clinet_SetTeamColorInPC_Implementation();

	ETeamColor TestColor = ETeamColor::None;

	UFUNCTION(Server, Reliable)
		void Server_SetTestColor(const ETeamColor& color);
	void Server_SetTestColor_Implementation(const ETeamColor& color);

	UFUNCTION(Client, Reliable)
		void Client_TestWidget();
	void Client_TestWidget_Implementation();


};
