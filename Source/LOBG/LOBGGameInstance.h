// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LOBGGameInstance.generated.h"

UENUM(BlueprintType)
enum class ETeamColor : uint8
{
	None =	0 UMETA(Display = "None"),
	Red =	1 UMETA(Display = "Red"),
	Blue =	2 UMETA(Display = "Blue"),
};

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		ETeamColor TeamColor = ETeamColor::None;

	TArray<FString> TeamRedUsers;
	TArray<FString> TeamBlueUsers;
};
