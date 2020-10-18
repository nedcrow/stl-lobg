// Fill out your copyright notice in the Description page of Project Settings.


#include "AIManager.h"

#include "AIMinionChar.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

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

}

void AAIManager::ReverseCoursePoints(TArray<AWaveCoursePoint*>& Array)
{
	int ArrayLength = Array.Num();
	if (ArrayLength > 1)		// �迭 ���� 2 �̻��̸� ���� ����.
	{
		for (int i = 0; i < ArrayLength / 2; i++)
		{
			Array.Swap(i, ArrayLength - 1 - i);		// �Է��� �迭�� �յڸ� ���� �ٲ㼭 ���� ������ �Ųٷ� �����Ѵ�.
		}
	}
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
				int ArrayIndex = CoursePoints.Num() - 1;		// ��ǥ ���� �迭�� ������ �ε���.

				if (ArrayIndex == -1)			// ù��°�� �˻��� ���Ҹ� �ٷ� �迭�� �߰��ȴ�.
				{
					CoursePoints.Add(CurrentElement);
				}
				else if (CurrentElement->WaveCourse >= CoursePoints[ArrayIndex]->WaveCourse)		// ������ ���Һ��� �ڽ��� ũ�ų� ������ �ڿ� �߰�.
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


	// Ÿ�� ����Ʈ ���� �����
	//FVector StartLast;
	//FVector GoalFirst;

	//for (int i = 0; i < CoursePoints.Num(); i++)
	//{
	//	for (int k = i + 1; k < CoursePoints.Num(); k++)
	//	{
	//		if (CoursePoints[i]->WaveCourse == CoursePoints[k]->WaveCourse)
	//		{
	//			UKismetSystemLibrary::DrawDebugLine(GetWorld(), CoursePoints[i]->GetActorLocation(), CoursePoints[k]->GetActorLocation(), FLinearColor::Blue, -1.f, 30.f);
	//		}
	//		else 
	//		{
	//			if (CoursePoints[i]->WaveCourse == EWaveCourse::SpawnPoint)
	//			{
	//				StartLast = CoursePoints[i]->GetActorLocation();
	//			}

	//			if (CoursePoints[k]->WaveCourse == EWaveCourse::Goal)
	//			{
	//				GoalFirst = CoursePoints[k]->GetActorLocation();
	//			}
	//			else
	//			{

	//			}

	//			
	//		}
	//	
	//	}
	//}
}

// BB�� ��ǥ ��ġ �ٲٱ�.
bool AAIManager::ChangeNextTarget(AAIController * AIController)
{
	if (AIController && CoursePoints.Num() > 0)		// ��Ʈ�ѷ�
	{
		AAIMinionChar* AIChar = AIController->GetPawn<AAIMinionChar>();

		if (AIChar)		// ĳ����. �ڽ�����Ʈ
		{
			if (!AIChar->CurrentMoveTarget)
			{
				AIChar->CurrentMoveTarget = CoursePoints[0];		// �ڽ� ����Ʈ�� ������� ��� ������������ �ٽ� ä���.
			}

			int Index = -1;
			CoursePoints.Find(AIChar->CurrentMoveTarget, Index);		// ���� �ڽ�����Ʈ ã��
			int NextIndex = Index + 1;
			if (Index >= 0 && NextIndex < CoursePoints.Num() && CoursePoints.IsValidIndex(NextIndex))		// ���� �ڽ�����Ʈ
			{
				UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
				if (BBComp)		// ������
				{
					if (CoursePoints[Index]->WaveCourse != CoursePoints[NextIndex]->WaveCourse)		// NextIndex�� �ڽ��� ���� ��쿡�� NextIndex�� �״�� �����ϰ� �ƴϸ� �ڽ��� ���� �ڽ��� ã�´�.
					{
						for (int i = NextIndex; i < CoursePoints.Num(); i++)
						{
							if ((int)CoursePoints[i]->WaveCourse == AIChar->WaveCourse)		// ���� �ڽ����� �ڽ��� �ڽ��� ����
							{
								NextIndex = i;
								break;
							}
							else if (CoursePoints[i]->WaveCourse == CoursePoints.Last()->WaveCourse)		// ������ �ڽ��� ����
							{
								NextIndex = i;
								break;
							}
							else if (i == CoursePoints.Num() - 1)		// ������ ����Ʈ ����
							{
								NextIndex = i;
								break;
							}
						}						
					}


					FVector RelativeLocation = CoursePoints[NextIndex]->GetActorLocation() - AIChar->CurrentMoveTarget->GetActorLocation();

					BBComp->SetValueAsVector(TEXT("CourseLocation"), AIChar->GetActorLocation() + RelativeLocation);		// �ڽ� �����̼� Ű ����

					AIChar->CurrentMoveTarget = CoursePoints[NextIndex];		// �ڽ�����Ʈ ����.

					return true;
				}
			}
		}
	}

	return false;
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

		WaveCourse = FMath::RandRange(1, 3);		// ���̺� ���� �Ҵ�.

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

	if (NewMinion)
	{
		// ������ ���� ����
		ActiveMinions.Add(NewMinion);
		
		// �� ����
		NewMinion->SetTeamName(TeamName);

		// ������ ���͵��� �̵� ��ǥ ����
		NewMinion->WaveCourse = WaveCourse;			// �ڽ� ����.

		AAIController* AIC = NewMinion->GetController<AAIController>();
		if (AIC)
		{
			ChangeNextTarget(AIC);		// BB Ű�� ��ǥ �����̼� �Է�
		}

		// ���� ����Ʈ ����

		LeftSpawnNumber--;		// ���� ����.
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

