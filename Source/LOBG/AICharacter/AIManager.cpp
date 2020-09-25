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

	SeachCoursePoints();		// ���̺� �ڽ� ����Ʈ ã�Ƽ� ����.


	// �׽�Ʈ
	SpawnMinions(3);
	
}

void AAIManager::SeachCoursePoints()
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaveCoursePoint::StaticClass(), OutActors);
	if (OutActors.Num() <= 0)
	{
		return;
	}

	AWaveCoursePoint* CurrentElement;		// �ӽ� ������
	CoursePoints.Empty();
	for (int i = 0; i < 5; i++)		// ������ �˻�
	{
		for (int j = 0; j < OutActors.Num(); j++)
		{
			CurrentElement = Cast<AWaveCoursePoint>(OutActors[j]);

			if ((int)CurrentElement->WaveCourse == i)		// �迭�� �������� ������� �˻��Ͽ� ���� �������� ã�´�.
			{
				int ArrayIndex = CoursePoints.Num() - 1;

				if (j == 0)			// �ش� �������� ù��°�� �ٷ� �߰�. ù��°�� �˻��� ���ҿ��� �ٷ� �迭�� �߰��ȴ�.
				{
					CoursePoints.Add(CurrentElement);
				}
				else if (CurrentElement->CourseNumber >= CoursePoints[ArrayIndex]->CourseNumber)		// ������ ���Һ��� �ڽ��ѹ��� ũ�ų� ������ �ڿ� �߰�.
				{
					CoursePoints.Add(CurrentElement);
				}
				else if (CurrentElement->CourseNumber < CoursePoints[ArrayIndex]->CourseNumber)		// ������ ���Һ��� �ڽ��ѹ��� ������ ���� �迭 ��ȣ�� ����.
				{
					CoursePoints.Insert(CurrentElement, ArrayIndex);
				}

				OutActors.RemoveAt(j);		// �迭���� �߰��� ���Ҹ� ����.
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
	UE_LOG(LogClass,Warning,TEXT("11111111111111111111111111111 %d"), CoursePoints.Num())

	// ���� ����Ʈ ã��
	FVector SpawnLocation = CoursePoints[0]->GetActorLocation();

	// ���� ����
	for (int i = MinionQuantity; i < MinionQuantity; i++)
	{
		AAIMinionChar* NewMinion = GetWorld()->SpawnActor<AAIMinionChar>(SpawnLocation, FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f));
		UE_LOG(LogClass, Warning, TEXT("222222222222222222222222222 %d"), CoursePoints.Num())
		// ������ ���� ����
		ActiveMinions.Add(NewMinion);
	}

	// ������ ���͵��� �̵� ��ǥ ����

}

