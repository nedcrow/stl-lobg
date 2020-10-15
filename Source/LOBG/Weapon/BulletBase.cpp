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
	StaticMesh->bCastDynamicShadow = false;		// Disallow mesh to cast dynamic shadows
	StaticMesh->CastShadow = false;			// Disallow mesh to cast other shadows

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));

	Tags.Add(TEXT("Bullet"));
}

// Called when the game starts or when spawned
void ABulletBase::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ABulletBase::OnBeginOverlap);
	Sphere->OnComponentHit.AddDynamic(this, &ABulletBase::OnHit);
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

void ABulletBase::SetDamageInfo(FHitResult OutHit, AController* Controller, float NewAttackPoint)
{
	TraceHit = OutHit;
	SummonerController = Controller;
	AttackPoint = NewAttackPoint;
}

//�÷��̾ �浹�ϸ�
void ABulletBase::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogClass, Warning, TEXT("OnBeginOverlap OtherComp  : %s"), *OtherComp->GetName());
	


	// �������̶�� return
	if (OtherActor->ActorHasTag(TeamName))
	{
		Destroy();
		return;
	}

	// ApplyDamage
	if (OtherActor->ActorHasTag(TEXT("Player")))
	{
		UE_LOG(LogClass, Warning, TEXT("LogThisCode"));
		UGameplayStatics::ApplyPointDamage(OtherActor, AttackPoint, -SweepResult.ImpactNormal, SweepResult, SummonerController, this, UBulletDamageType::StaticClass());
		Destroy();
	}
	else if (OtherActor->ActorHasTag(TEXT("Minion")))
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, AttackPoint, -SweepResult.ImpactNormal, SweepResult, SummonerController, this, UBulletDamageType::StaticClass());
		Destroy();
	}
	else if (OtherActor->ActorHasTag(TEXT("Tower")))
	{
		UGameplayStatics::ApplyDamage(OtherActor, AttackPoint, SummonerController, this, UBulletDamageType::StaticClass());
		Destroy();
	}

	//������ �÷��̾ �ƴ� �ٸ� ���Ͷ�� �������.
	//������ ������� �ʵ��� Bullet �±׸� �˻��Ѵ�.
	else if (!OtherActor->ActorHasTag(TEXT("Bullet")) && !OtherComp->ComponentHasTag(TEXT("Weapon")))
	{
		UE_LOG(LogClass, Warning, TEXT("Other Actor : %s"), *OtherActor->GetName());
		Destroy();
	}	
}

void ABulletBase::OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	// Sound
	// Effect Multicast_SpawnHitEffectAndDecal(OutHit);
	Destroy();
}

