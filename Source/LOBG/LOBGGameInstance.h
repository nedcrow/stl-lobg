// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LOBGGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class LOBG_API ULOBGGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	FString UserID;

	FString GetUserID();
	void SetUserID(FString NewUserID);
};
