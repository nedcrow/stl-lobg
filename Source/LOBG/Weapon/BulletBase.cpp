// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Weapon/BulletDamageType.h"
#include "../Battle/BattlePC.h"
#include "../Battle/BattleCharacter.h"

// Sets default values
ABulletBase::ABulletBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = Sphere;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));

	Tags.Add(TEXT("Bullet"));
}

// Called when the game starts or when spawned
void ABulletBase::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ABulletBase::OnBeginOverlap);
}

// Called every frame
void ABulletBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABulletBase::ApplyDamageProcess(EApplyDamageType ApplyDamageType)
{
	switch (ApplyDamageType)
	{
	case EApplyDamageType::Player:
		UGameplayStatics::ApplyPointDamage(PlayerOutHit.GetActor(), 1.0f, -PlayerOutHit.ImpactNormal, PlayerOutHit, PlayerController, this, UBulletDamageType::StaticClass());
		break;
	case EApplyDamageType::Minion:
	case EApplyDamageType::Tower:
		UGameplayStatics::ApplyDamage(PlayerOutHit.GetActor(), 10.0f, PlayerController, this, UBulletDamageType::StaticClass());
		break;
	default:
		break;
	}


	//사운드

	//이펙트

	Destroy();
}

void ABulletBase::SetDamageInfo(FHitResult OutHit, AController* Controller)
{
	PlayerOutHit = OutHit;
	PlayerController = Controller;
}

void ABulletBase::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	ABattlePC* PC = Cast<ABattlePC>(PlayerController);
	if (PC)
	{
		ABattleCharacter* PlayerPawn = Cast<ABattleCharacter>(PC->GetPawn());
		if (PlayerPawn)
		{
			if (OtherActor->ActorHasTag(PlayerPawn->TeamName))
			{
				return;
			}
		}
	}

	//플레이어에 충돌하면
	if (OtherActor->ActorHasTag(TEXT("Player")))
	{
		CurrentDamageType = EApplyDamageType::Player;
		//데미지 전달
		ApplyDamageProcess(CurrentDamageType);
	}
	else if (OtherActor->ActorHasTag(TEXT("Minion")))
	{
		CurrentDamageType = EApplyDamageType::Minion;
		ApplyDamageProcess(CurrentDamageType);
	}
	else if (OtherActor->ActorHasTag(TEXT("Tower")))
	{
		CurrentDamageType = EApplyDamageType::Tower;
		ApplyDamageProcess(CurrentDamageType);
	}

	//맞은게 플레이어가 아닌 다른 액터라면 사라진다.
	//스스로 사라지지 않도록 Bullet태그를 검사한다.
	else if(!OtherActor->ActorHasTag(TEXT("Bullet")))
	{
		UE_LOG(LogClass, Warning, TEXT("Other Actor : %s"), *OtherActor->GetName());
		Destroy();
	}
}

