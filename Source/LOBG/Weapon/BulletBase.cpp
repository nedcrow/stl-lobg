// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Weapon/BulletDamageType.h"
#include "../Battle/BattlePC.h"
#include "../Battle/BattleCharacter.h"
#include "../AICharacter/Fairy/FairyPawn.h"
#include "../AICharacter/AIMinionChar.h"
#include "Net/UnrealNetwork.h"

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

void ABulletBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABulletBase, TeamName);
}

void ABulletBase::SetDamageInfo(FHitResult OutHit, AController* Controller)
{
	TraceHit = OutHit;
	SummonerController = Controller;
}

void ABulletBase::ApplyDamageProcess(EApplyDamageType ApplyDamageType)
{
	switch (ApplyDamageType)
	{
	case EApplyDamageType::Player:
		UGameplayStatics::ApplyPointDamage(TraceHit.GetActor(), GetAttackPoint(), -TraceHit.ImpactNormal, TraceHit, SummonerController, this, UBulletDamageType::StaticClass());
		break;
	case EApplyDamageType::Minion:
		UGameplayStatics::ApplyDamage(TraceHit.GetActor(), GetAttackPoint(), SummonerController, this, UBulletDamageType::StaticClass());
		break;
	case EApplyDamageType::Tower:
		UGameplayStatics::ApplyDamage(TraceHit.GetActor(), GetAttackPoint(), SummonerController, this, UBulletDamageType::StaticClass());
		break;
	default:
		break;
	}

	//사운드

	//이펙트 Multicast_SpawnHitEffectAndDecal(OutHit);

	Destroy();
}

float ABulletBase::GetAttackPoint()
{
	float AP = 0.f;
	if (SummonerController) {
		AActor* Pawn = SummonerController->GetViewTarget();
		if (Pawn) {
			if (Pawn->ActorHasTag(TEXT("Player"))) {
				AP = Cast<ABattleCharacter>(Pawn)->AttackPoint ? Cast<ABattleCharacter>(Pawn)->AttackPoint : 0.f;
			}
			else if (Pawn->ActorHasTag(TEXT("Minion"))) {
				AP = Cast<AAIMinionChar>(Pawn)->AttackDamage ? Cast<AAIMinionChar>(Pawn)->AttackDamage : 0.f;
			}
			else if (Pawn->ActorHasTag(TEXT("Tower"))) {
				AP = Cast<AFairyPawn>(Pawn)->AttackPoint ? Cast<AFairyPawn>(Pawn)->AttackPoint : 0.f;
			}
		}
	}

	return AP;
}

void ABulletBase::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogClass, Warning, TEXT("OnBeginOverlap  : %s"), *OtherActor->GetName());


	//같은팀이라면 return
	if (OtherActor->ActorHasTag(TeamName))
	{
	UE_LOG(LogTemp,Warning,TEXT("TeamKill? Really?"));
		Destroy();
		return;
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
	//스스로 사라지지 않도록 Bullet 태그를 검사한다.
	else if (!OtherActor->ActorHasTag(TEXT("Bullet")) && !OtherComp->ComponentHasTag(TEXT("Weapon")))
	{
		UE_LOG(LogClass, Warning, TEXT("Other Actor : %s"), *OtherActor->GetName());
		Destroy();
	}
}

