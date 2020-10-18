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

	SeachCoursePoints();		// 웨이브 코스 포인트를 찾아서 저장.

}

void AAIManager::ReverseCoursePoints(TArray<AWaveCoursePoint*>& Array)
{
	int ArrayLength = Array.Num();
	if (ArrayLength > 1)		// 배열 길이 2 이상이면 스왑 실행.
	{
		for (int i = 0; i < ArrayLength / 2; i++)
		{
			Array.Swap(i, ArrayLength - 1 - i);		// 입력한 배열을 앞뒤를 서로 바꿔서 정렬 순서를 거꾸로 저장한다.
		}
	}
}

// CoursePoints 찾기
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
				int ArrayIndex = CoursePoints.Num() - 1;		// 목표 저장 배열의 마지막 인덱스.

				if (ArrayIndex == -1)			// 첫번째로 검색한 원소면 바로 배열에 추가된다.
				{
					CoursePoints.Add(CurrentElement);
				}
				else if (CurrentElement->WaveCourse >= CoursePoints[ArrayIndex]->WaveCourse)		// 마지막 원소보다 코스가 크거나 같으면 뒤에 추가.
				{
					CoursePoints.Add(CurrentElement);
				}
				else if (CurrentElement->CourseNumber >= CoursePoints[ArrayIndex]->CourseNumber)		// 마지막 원소보다 코스넘버가 크거나 같으면 뒤에 추가.
				{
					CoursePoints.Add(CurrentElement);
				}
				else 	// 마지막 원소보다 코스넘버가 작으면 앞쪽 배열을 검색하여 코스넘버를 비교하여 삽입할 위치를 찾는다.
				{
					while (true)
					{
						ArrayIndex--;
						if (ArrayIndex <= 0			// 배열의 가장 앞일 경우. 이미 이전 루프에서 조건 검사를 했기 때문에 바로 삽입한다.
							|| (int)CoursePoints[ArrayIndex - 1]->WaveCourse != i			// 한칸 앞 원소가 다른 코스일 경우
							|| CurrentElement->CourseNumber >= CoursePoints[ArrayIndex - 1]->CourseNumber)		// 한칸 앞 원소의 코스넘버가 작을 경우
						{
							CoursePoints.Insert(CurrentElement, ArrayIndex);
							break;
						}
					}
				}

				OutActors.RemoveAt(j--);		// 배열에서 추가한 원소를 제거.
			}
		}
	}


	// 타겟 포인트 라인 만들기
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

// BB의 목표 위치 바꾸기.
bool AAIManager::ChangeNextTarget(AAIController * AIController)
{
	if (AIController && CoursePoints.Num() > 0)		// 컨트롤러
	{
		AAIMinionChar* AIChar = AIController->GetPawn<AAIMinionChar>();

		if (AIChar)		// 캐릭터. 코스포인트
		{
			if (!AIChar->CurrentMoveTarget)
			{
				AIChar->CurrentMoveTarget = CoursePoints[0];		// 코스 포인트가 비어있을 경우 시작지점으로 다시 채운다.
			}

			int Index = -1;
			CoursePoints.Find(AIChar->CurrentMoveTarget, Index);		// 현재 코스포인트 찾기
			int NextIndex = Index + 1;
			if (Index >= 0 && NextIndex < CoursePoints.Num() && CoursePoints.IsValidIndex(NextIndex))		// 다음 코스포인트
			{
				UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
				if (BBComp)		// 블랙보드
				{
					if (CoursePoints[Index]->WaveCourse != CoursePoints[NextIndex]->WaveCourse)		// NextIndex의 코스가 같은 경우에는 NextIndex를 그대로 유지하고 아니면 자신의 다음 코스를 찾는다.
					{
						for (int i = NextIndex; i < CoursePoints.Num(); i++)
						{
							if ((int)CoursePoints[i]->WaveCourse == AIChar->WaveCourse)		// 시작 코스에서 자신의 코스로 진입
							{
								NextIndex = i;
								break;
							}
							else if (CoursePoints[i]->WaveCourse == CoursePoints.Last()->WaveCourse)		// 마지막 코스로 진입
							{
								NextIndex = i;
								break;
							}
							else if (i == CoursePoints.Num() - 1)		// 마지막 포인트 진입
							{
								NextIndex = i;
								break;
							}
						}						
					}


					FVector RelativeLocation = CoursePoints[NextIndex]->GetActorLocation() - AIChar->CurrentMoveTarget->GetActorLocation();

					BBComp->SetValueAsVector(TEXT("CourseLocation"), AIChar->GetActorLocation() + RelativeLocation);		// 코스 로케이션 키 변경

					AIChar->CurrentMoveTarget = CoursePoints[NextIndex];		// 코스포인트 저장.

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

// 스폰 설정 및 시작
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

	if (LeftSpawnNumber <= 0)		// 스폰 횟수 체크
	{
		// 스폰 횟수 갱신
		LeftSpawnNumber = MinionQuantity;

		// 스폰 포인트 찾기
		CurrentSpawnLocation = CoursePoints[0]->GetActorLocation();

		CurrentRotatingNumber = 0;		// 스폰 회전 위치 초기화

		WaveCourse = FMath::RandRange(1, 3);		// 웨이브 랜덤 할당.

		// 액터 스폰 시작
		RepeatSpawnMinions();
	}
	else
	{
		// 스폰 횟수 추가
		LeftSpawnNumber += MinionQuantity;
	}

}

// 스폰 머신
void AAIManager::RepeatSpawnMinions()
{
	// 스폰 위치 조정
	if (CurrentRotatingNumber <= 0)		// 숫자가 0 이하면 초기화
	{
		CurrentRotatingNumber = 0;
		CurrentRotatingAngle = 0.f;
	}
	else
	{
		CurrentRotatingAngle += 60.f / CurrentRotatingNumber;		// 각도 변경
		if (CurrentRotatingAngle >= 360.f)		// 회전이 360도 이상이면 다음 회전으로 이동하고 0도부터 시작.
		{
			CurrentRotatingNumber++;

			if (CurrentRotatingNumber > 3)		// 3단계까지 스폰하면 다시 원점으로 돌아간다. 총 37개 생성 (1, 6, 12, 18).
			{
				CurrentRotatingNumber = 0;
			}

			CurrentRotatingAngle = 0.f;
		}
	}

	// 현재 스폰 포인트 위치 + 캡슐 절반 높이 + 스폰 위치 상대 벡터
	FVector NewSpawnLocation = CurrentSpawnLocation + FVector(0.f, 0.f, 88.f) + FRotator(0.f, CurrentRotatingAngle, 0.f).Vector() * (200.f * CurrentRotatingNumber);

	// 액터 스폰
	AAIMinionChar* NewMinion = GetWorld()->SpawnActor<AAIMinionChar>(MinionCharClass, NewSpawnLocation, FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f));

	if (NewMinion)
	{
		// 스폰한 액터 저장
		ActiveMinions.Add(NewMinion);
		
		// 팀 배정
		NewMinion->SetTeamName(TeamName);

		// 스폰한 액터들의 이동 목표 설정
		NewMinion->WaveCourse = WaveCourse;			// 코스 저장.

		AAIController* AIC = NewMinion->GetController<AAIController>();
		if (AIC)
		{
			ChangeNextTarget(AIC);		// BB 키에 목표 로케이션 입력
		}

		// 스폰 이펙트 생성

		LeftSpawnNumber--;		// 스폰 성공.
	}



	// 스폰 Repeat
	if (LeftSpawnNumber > 0)
	{
		GetWorldTimerManager().SetTimer(RepeatSpawnHandle, this, &AAIManager::RepeatSpawnMinions, 0.5f, false);

		if (CurrentRotatingNumber == 0)
		{
			CurrentRotatingNumber = 1;
		}
	}
}

