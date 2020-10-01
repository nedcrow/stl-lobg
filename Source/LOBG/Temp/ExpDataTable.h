// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ExpDataTable.generated.h"



USTRUCT(BlueprintType)
struct LOBG_API FExpDataTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Level;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString ExpforNextLevel;
};
