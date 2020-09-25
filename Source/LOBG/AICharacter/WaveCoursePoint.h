// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "WaveCoursePoint.generated.h"

UENUM(BlueprintType)
enum class EWaveCourse : uint8
{
	SpawnPoint = 0 UMETA(DisplayName = "SpawnPoint"),
	CourseA = 1 UMETA(DisplayName = "CourseA"),
	CourseB = 2 UMETA(DisplayName = "CourseB"),
	CourseC = 3 UMETA(DisplayName = "CourseC"),
	Goal = 4 UMETA(DisplayName = "Goal")
};


/**
 * 
 */
UCLASS()
class LOBG_API AWaveCoursePoint : public ATargetPoint
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		EWaveCourse WaveCourse;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		uint8  CourseNumber = 0;

};
