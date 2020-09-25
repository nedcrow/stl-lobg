// Fill out your copyright notice in the Description page of Project Settings.


#include "AIManager.h"

#include "AIMinionChar.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AAIManager::AAIManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAIManager::BeginPlay()
{
	Super::BeginPlay();

	SeachCoursePoints();		// 웨이브 코스 포인트 찾아서 저장.


	// 테스트
	SpawnMinions(1);
	
}

void AAIManager::SeachCoursePoints()
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaveCoursePoint::StaticClass(), OutActors);
	if (OutActors.Num() <= 0)
	{
		return;
	}

	AWaveCoursePoint* CurrentElement;		// 임시 포인터
	CoursePoints.Empty();
	for (int i = 0; i < 5; i++)		// 열거형 검색
	{
		for (int j = 0; j < OutActors.Num(); j++)
		{
			CurrentElement = Cast<AWaveCoursePoint>(OutActors[j]);

			if ((int)CurrentElement->WaveCourse == i)		// 배열의 열거형을 순서대로 검색하여 같은 열거형을 찾는다.
			{
				int ArrayIndex = CoursePoints.Num() - 1;

				if (j == 0)			// 해당 열거형의 첫번째면 바로 추가. 첫번째로 검색한 원소여도 바로 배열에 추가된다.
				{
					CoursePoints.Add(CurrentElement);
				}
				else if (CurrentElement->CourseNumber >= CoursePoints[ArrayIndex]->CourseNumber)		// 마지막 원소보다 코스넘버가 크거나 같으면 뒤에 추가.
				{
					CoursePoints.Add(CurrentElement);
				}
				else if (CurrentElement->CourseNumber < CoursePoints[ArrayIndex]->CourseNumber)		// 마지막 원소보다 코스넘버가 작으면 현재 배열 번호에 삽입.
				{
					CoursePoints.Insert(CurrentElement, ArrayIndex);
				}

				OutActors.RemoveAt(j--);		// 배열에서 추가한 원소를 제거.
			}
		}
	}
	
}

// Called every frame
void AAIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// MinionsArray
void AAIManager::AddMinion(AAIMinionChar * AddMinion)
{
	if (AddMinion)
	{
		ActiveMinions.Add(AddMinion);
	}
}

void AAIManager::RemoveMinion(AAIMinionChar * RemoveMinion)
{
	if (ActiveMinions.Num() > 0 && RemoveMinion)
	{
		ActiveMinions.Remove(RemoveMinion);
	}
}

void AAIManager::SpawnMinions(int MinionQuantity)
{
	if (MinionQuantity < 1 || CoursePoints.Num() <= 0)
	{
		return;
	}

	// 스폰 포인트 찾기
	FVector SpawnLocation = CoursePoints[0]->GetActorLocation();

	// 액터 스폰
	for (int i = 0; i < MinionQuantity; i++)
	{
		FActorSpawnParameters aaa;
		aaa.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AAIMinionChar* NewMinion = GetWorld()->SpawnActor<AAIMinionChar>(MinionCharClass, FTransform(FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f), SpawnLocation));
		//AAIMinionChar* NewMinion = GetWorld()->SpawnActorDeferred<AAIMinionChar>(MinionCharClass, FTransform(FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f), SpawnLocation), nullptr,nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

		// 스폰한 액터 저장
		ActiveMinions.Add(NewMinion);
	}

	// 스폰한 액터들의 이동 목표 설정

}

