// Fill out your copyright notice in the Description page of Project Settings.


#include "EmissiveBullet.h"

#include "../Weapon/BulletDamageType.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEmissiveBullet::AEmissiveBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->OnComponentHit.AddDynamic(this, &AEmissiveBullet::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// StaticMesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(CollisionComp);
	StaticMesh->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMesh->bCastDynamicShadow = false;		// Disallow mesh to cast dynamic shadows
	StaticMesh->CastShadow = false;			// Disallow mesh to cast other shadows

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	// ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	// Network. 사용하지 않는다. 총알을 각자 스폰해야 전송량을 줄일 수 있다.
	//bReplicates = true;

	// ETC
	Tags.Add(TEXT("Bullet"));
}

// Called when the game starts or when spawned
void AEmissiveBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEmissiveBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Network
void AEmissiveBullet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AEmissiveBullet, TeamName);
}

// InitBulletInfo
void AEmissiveBullet::SetDamageInfo(AController* Controller, float NewAttackPoint, FName NewTeamName)
{
	SummonerController = Controller;
	AttackPoint = NewAttackPoint;
	TeamName = NewTeamName;
	Tags.Add(NewTeamName);
}

// Hit
void AEmissiveBullet::OnHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	//같은팀이라면 return
	if (OtherActor == NULL || OtherActor->ActorHasTag(TeamName))
	{
		Destroy();
		return;
	}
	else if (SummonerController && OtherActor == SummonerController->GetPawn())
	{
		return;
	}


	// 데미지는 서버에서만 계산한다.
	if (GetWorld()->IsServer())
	{
		//플레이어에 충돌하면
		if (OtherActor->ActorHasTag(TEXT("Player")))
		{
			//UGameplayStatics::ApplyPointDamage(OtherActor, AttackPoint, -Hit.ImpactNormal, Hit, SummonerController, this, UBulletDamageType::StaticClass());
			UGameplayStatics::ApplyDamage(OtherActor, AttackPoint, SummonerController, this, UBulletDamageType::StaticClass());
		}
		else if (OtherActor->ActorHasTag(TEXT("Minion")))
		{
			UGameplayStatics::ApplyDamage(OtherActor, AttackPoint, SummonerController, this, UBulletDamageType::StaticClass());
		}
		else if (OtherActor->ActorHasTag(TEXT("Tower")))
		{
			UGameplayStatics::ApplyDamage(OtherActor, AttackPoint, SummonerController, this, UBulletDamageType::StaticClass());
		}
	}
	else if(!GetWorld()->IsServer() && OtherActor->ActorHasTag(TEXT("Minion")))
	{
		// 클라이언트 피격 이펙트용 가짜 데미지.
		UGameplayStatics::ApplyDamage(OtherActor, AttackPoint, SummonerController, this, UBulletDamageType::StaticClass());
	}

	Destroy();
}

