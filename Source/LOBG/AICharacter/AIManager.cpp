// Fill out your copyright notice in the Description page of Project Settings.


#include "AIManager.h"

#include "AIMinionChar.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

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

	SeachCoursePoints();		// ���̺� �ڽ� ����Ʈ�� ã�Ƽ� ����.

	// Test
	SetSpawnQuantity(30);
}

// CoursePoints ã��
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
				else 	// ������ ���Һ��� �ڽ��ѹ��� ������ ���� �迭�� �˻��Ͽ� �ڽ��ѹ��� ���Ͽ� ������ ��ġ�� ã�´�.
				{
					while (true)
					{
						ArrayIndex--;
						if (ArrayIndex <= 0			// �迭�� ���� ���� ���. �̹� ���� �������� ���� �˻縦 �߱� ������ �ٷ� �����Ѵ�.
							|| (int)CoursePoints[ArrayIndex - 1]->WaveCourse != i			// ��ĭ �� ���Ұ� �ٸ� �ڽ��� ���
							|| CurrentElement->CourseNumber >= CoursePoints[ArrayIndex - 1]->CourseNumber)		// ��ĭ �� ������ �ڽ��ѹ��� ���� ���
						{
							CoursePoints.Insert(CurrentElement, ArrayIndex);
							break;
						}
					}
				}

				OutActors.RemoveAt(j--);		// �迭���� �߰��� ���Ҹ� ����.
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

// ���� ���� �� ����
void AAIManager::SetSpawnQuantity(int MinionQuantity)
{
	if (MinionQuantity < 1 || CoursePoints.Num() <= 0)
	{
		return;
	}

	UE_LOG(LogClass, Warning, TEXT("-------CoursePoints.Num()-------- %d"), CoursePoints.Num());
	for (int i = 0; i < CoursePoints.Num(); i++)
	{
		UE_LOG(LogClass, Warning, TEXT("-----WaveCourse------- %d"), CoursePoints[i]->WaveCourse);
		UE_LOG(LogClass, Warning, TEXT("----CourseNumber------ %d"), CoursePoints[i]->CourseNumber);
	}

	if (LeftSpawnNumber <= 0)		// ���� Ƚ�� üũ
	{
		// ���� Ƚ�� ����
		LeftSpawnNumber = MinionQuantity;

		// ���� ����Ʈ ã��
		CurrentSpawnLocation = CoursePoints[0]->GetActorLocation();

		CurrentRotatingNumber = 0;		// ���� ȸ�� ��ġ �ʱ�ȭ

		// ���� ���� ����
		RepeatSpawnMinions();
	}
	else
	{
		// ���� Ƚ�� �߰�
		LeftSpawnNumber += MinionQuantity;
	}

}

// ���� �ӽ�
void AAIManager::RepeatSpawnMinions()
{
	// ���� ��ġ ����
	if (CurrentRotatingNumber <= 0)		// ���ڰ� 0 ���ϸ� �ʱ�ȭ
	{
		CurrentRotatingNumber = 0;
		CurrentRotatingAngle = 0.f;
	}
	else
	{
		CurrentRotatingAngle += 60.f / CurrentRotatingNumber;		// ���� ����
		if (CurrentRotatingAngle >= 360.f)		// ȸ���� 360�� �̻��̸� ���� ȸ������ �̵��ϰ� 0������ ����.
		{
			CurrentRotatingNumber++;

			if (CurrentRotatingNumber > 3)		// 3�ܰ���� �����ϸ� �ٽ� �������� ���ư���. �� 37�� ���� (1, 6, 12, 18).
			{
				CurrentRotatingNumber = 0;
			}

			CurrentRotatingAngle = 0.f;
		}
	}

	// ���� ���� ����Ʈ ��ġ + ĸ�� ���� ���� + ���� ��ġ ��� ����
	FVector NewSpawnLocation = CurrentSpawnLocation + FVector(0.f, 0.f, 88.f) + FRotator(0.f, CurrentRotatingAngle, 0.f).Vector() * (200.f * CurrentRotatingNumber);

	// ���� ����
	AAIMinionChar* NewMinion = GetWorld()->SpawnActor<AAIMinionChar>(MinionCharClass, NewSpawnLocation, FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f));

	// ������ ���� ����
	if (NewMinion)
	{
		ActiveMinions.Add(NewMinion);
		LeftSpawnNumber--;

		// ������ ���͵��� �̵� ��ǥ ����

		// ���� ����Ʈ ����

	}



	// ���� Repeat
	if (LeftSpawnNumber > 0)
	{
		GetWorldTimerManager().SetTimer(RepeatSpawnHandle, this, &AAIManager::RepeatSpawnMinions, 0.5f, false);

		if (CurrentRotatingNumber == 0)
		{
			CurrentRotatingNumber = 1;
		}
	}
}

