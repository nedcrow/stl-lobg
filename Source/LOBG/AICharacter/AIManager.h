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
	void SeachCoursePoints();		// �ڽ� ����Ʈ �˻� �� �迭�� ����.
	void SeachCoursePoints2();		// �ڽ� ����Ʈ �˻� �� �迭�� ����. (���� �߰�)
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
	int WaveCourse;		// ������ �ڽ� ��ȣ.
	UFUNCTION(BlueprintCallable)
		void SetSpawnQuantity(int MinionQuantity);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		int LeftSpawnNumber = 0;

	FVector CurrentSpawnLocation;	
	int CurrentRotatingNumber = 0;		// 360�� ȸ�� Ƚ��
	float CurrentRotatingAngle = 0.f;		// ȸ�� ����
	FTimerHandle RepeatSpawnHandle;

	UFUNCTION()
		void RepeatSpawnMinions();



};
