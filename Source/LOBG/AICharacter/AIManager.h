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

	void SeachCoursePoints();		// �ڽ� ����Ʈ �˻� �� �迭�� ����.
	bool ChangeNextTarget(class AAIController* AIController);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// MinionsArray
	TArray<AAIMinionChar*> ActiveMinions;

	void AddMinion(AAIMinionChar* AddMinion);
	void RemoveMinion(AAIMinionChar* RemoveMinion);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnData")
		TSubclassOf<class AAIMinionChar> MinionCharClass;

	// SpawnMinion
	UFUNCTION(BlueprintCallable)
		void SetSpawnQuantity(int MinionQuantity);

	int LeftSpawnNumber = 0;
	FVector CurrentSpawnLocation;
	int CurrentRotatingNumber = 0;		// 360�� ȸ�� Ƚ��
	float CurrentRotatingAngle = 0.f;		// ȸ�� ����
	FTimerHandle RepeatSpawnHandle;

	UFUNCTION()
		void RepeatSpawnMinions();


};
