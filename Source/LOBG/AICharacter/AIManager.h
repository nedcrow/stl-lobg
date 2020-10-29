// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "WaveCoursePoint.h"

#include "AIManager.generated.h"

class AAIMinionChar;

UCLASS()
class LOBG_API AAIManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAIManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// CoursePointsArray
	TArray<AWaveCoursePoint*> CoursePoints;

	void ReverseCoursePoints();
	void SeachCoursePoints();		// 코스 포인트 검색 후 배열에 저장.
	void SeachCoursePoints2();		// 코스 포인트 검색 후 배열에 저장. (조건 추가)
	bool ChangeNextTarget(class AAIController* AIController);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// TeamInfo
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		FName TeamName;

	// MinionsArray
	TArray<AAIMinionChar*> ActiveMinions;
	TArray<AAIController*> ReadyMinionControlls;
	
	void AddMinion(AAIMinionChar* AddMinion);
	void RemoveMinion(AAIMinionChar* RemoveMinion);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnData")
		TSubclassOf<class AAIMinionChar> MinionCharClass;

	// SpawnMinion
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Data")
	int WaveCourse;		// 배정할 코스 번호.
	UFUNCTION(BlueprintCallable)
		void SetSpawnQuantity(int MinionQuantity);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		int LeftSpawnNumber = 0;

	FVector CurrentSpawnLocation;	
	int CurrentRotatingNumber = 0;		// 360도 회전 횟수
	float CurrentRotatingAngle = 0.f;		// 회전 각도
	FTimerHandle RepeatSpawnHandle;

	UFUNCTION()
		void RepeatSpawnMinions();



};
