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
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/Actor.h"


// Sets default values
ABulletBase::ABulletBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = Sphere;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);
	StaticMesh->bCastDynamicShadow = false;		// Disallow mesh to cast dynamic shadows
	StaticMesh->CastShadow = false;			// Disallow mesh to cast other shadows

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	// Die after 3 seconds by default
	InitialLifeSpan = 5.f;

	Tags.Add(TEXT("Bullet"));
}

// Called when the game starts or when spawned
void ABulletBase::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ABulletBase::OnBeginOverlap);
	Sphere->OnComponentHit.AddDynamic(this, &ABulletBase::OnHit);
	if (FireSound) {
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(),
			FireSound,
			GetActorLocation()
		);
	}
	//SetLifeSpan(LifeSpanTime);
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

void ABulletBase::SetDamageInfo(AController* Controller, float NewAttackPoint, float NewAttackRadial, FName NewTeamName)
{
	//TraceHit = OutHit;
	SummonerController = Controller;
	AttackPoint = NewAttackPoint;
	AttackRadial = NewAttackRadial;
	TeamName = NewTeamName;
	Tags.Add(NewTeamName);
}

// HitEffect
void ABulletBase::NetMulticast_HitEffect_Implementation(FVector SpawnLocation, FVector HitDirection)
{
	if (HitEffect)
	{
		FRotator Rotation = HitDirection.Rotation();
		//UParticleSystemComponent* Particle =
		HitEffectComponent = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitEffect,
			SpawnLocation,
			Rotation,
			HitEffectScale);		

		GetWorldTimerManager().SetTimer(BulletTimer, this, &ABulletBase::DestroyHitEffect, 1.5f, false);
	}
	if (HitSound) {
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(),
			HitSound,
			SpawnLocation
		);
	}
}

void ABulletBase::DestroyHitEffect()
{
	if (HitEffectComponent && !HitEffectComponent->IsBeingDestroyed())
	{
		HitEffectComponent->DestroyComponent();
	}
}

//플레이어에 충돌하면
void ABulletBase::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogClass, Warning, TEXT("Other Actor : %s %s"), *OtherActor->GetName(), *SweepResult.BoneName.ToString());

	if (!bCanHit)
	{
		//공격 판정은 한번만 발생. 관통 공격이 추가될 경우 수정할 필요가 있다.
		return;
	}
	else if (OtherActor == NULL || OtherActor->ActorHasTag(TeamName))
	{
		// 같은팀이라면 return
		//NetMulticast_HitEffect(OtherActor->GetActorLocation());
		Destroy();
		return;
	}
	else if (SummonerController && OtherActor == SummonerController->GetPawn())
	{
		// 총알을 스폰한 당사자면 무시
		return;
	}

	bCanHit = false;

	// ApplyDamage
	if (GetWorld()->IsServer())		// 데미지는 서버에서만 계산한다.
	{
		// Radial effect 만 동기화
		if (AttackRadial > 0.f)
		{
			UGameplayStatics::ApplyRadialDamage(
				GetWorld(),
				AttackPoint,
				//OtherActor->GetActorLocation(), 
				SweepResult.ImpactPoint,
				AttackRadial,
				UBulletDamageType::StaticClass(),
				TArray<AActor*>(),
				this,
				SummonerController,
				true,
				ECC_Visibility);

			NetMulticast_HitEffect(SweepResult.ImpactPoint + SweepResult.ImpactNormal * 20.f, -SweepResult.ImpactNormal);
		}
		else 
		{
			if (OtherActor->ActorHasTag(TEXT("Player")))
			{
				UGameplayStatics::ApplyPointDamage(OtherActor, AttackPoint, -SweepResult.ImpactNormal, SweepResult, SummonerController, this, UBulletDamageType::StaticClass());

			}
			else if (OtherActor->ActorHasTag(TEXT("Minion")))
			{
				UGameplayStatics::ApplyPointDamage(OtherActor, AttackPoint, -SweepResult.ImpactNormal, SweepResult, SummonerController, this, UBulletDamageType::StaticClass());

			}
			else if (OtherActor->ActorHasTag(TEXT("Tower")))
			{
				UGameplayStatics::ApplyDamage(OtherActor, AttackPoint, SummonerController, this, UBulletDamageType::StaticClass());
			}

			NetMulticast_HitEffect_Implementation(SweepResult.ImpactPoint + GetVelocity().GetSafeNormal() * -20.f, -SweepResult.ImpactNormal);
		}
	}
	else if (!GetWorld()->IsServer())
	{
		// 클라이언트 미니언 피격 이펙트용 가짜 데미지.
		if (AttackRadial <= 0.f) {
			if (OtherActor->ActorHasTag(TEXT("Minion")))
			{
				// 클라이언트 미니언 피격 이펙트용 가짜 데미지.
				UGameplayStatics::ApplyDamage(OtherActor, AttackPoint, SummonerController, this, UBulletDamageType::StaticClass());
			}

			NetMulticast_HitEffect_Implementation(SweepResult.ImpactPoint + GetVelocity().GetSafeNormal() * -20.f, -SweepResult.ImpactNormal);
		}
	}

	//GetWorldTimerManager().SetTimer(BulletTimer, this, &ABulletBase::DestroyHitEffect, 1.5f, false);
	Destroy();
}

void ABulletBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bCanHit)
	{
		//공격 판정은 한번만 발생. 관통 공격이 추가될 경우 수정할 필요가 있다.
		return;
	}
	else if (OtherActor == NULL || OtherActor->ActorHasTag(TeamName))
	{
		// 같은팀이라면 return
		//NetMulticast_HitEffect(OtherActor->GetActorLocation());
		Destroy();
		return;
	}
	else if (SummonerController && OtherActor == SummonerController->GetPawn())
	{
		// 총알을 스폰한 당사자면 무시
		return;
	}

	bCanHit = false;

	// ApplyDamage
	if (GetWorld()->IsServer())		// 데미지는 서버에서만 계산한다.
	{
		// Radial effect 만 동기화
		if (AttackRadial > 0.f)
		{
			UGameplayStatics::ApplyRadialDamage(
				GetWorld(),
				AttackPoint,
				//OtherActor->GetActorLocation(), 
				Hit.ImpactPoint,
				AttackRadial,
				UBulletDamageType::StaticClass(),
				TArray<AActor*>(),
				this,
				SummonerController,
				true,
				ECC_Visibility
			);
			NetMulticast_HitEffect(Hit.ImpactPoint + Hit.ImpactNormal * 20.f, -Hit.ImpactNormal);
		}
		else
		{
			NetMulticast_HitEffect_Implementation(Hit.ImpactPoint + GetVelocity().GetSafeNormal() * -20.f, -Hit.ImpactNormal);
		}
	}
	else if (!GetWorld()->IsServer())
	{	
		if (AttackRadial <= 0.f)
		{
			if (OtherActor->ActorHasTag(TEXT("Minion")))
			{
				// 클라이언트 미니언 피격 이펙트용 가짜 데미지.
				UGameplayStatics::ApplyDamage(OtherActor, AttackPoint, SummonerController, this, UBulletDamageType::StaticClass());
			}

			NetMulticast_HitEffect_Implementation(Hit.ImpactPoint + GetVelocity().GetSafeNormal() * -20.f, -Hit.ImpactNormal);
		}
	}


	// Sound
	//GetWorldTimerManager().SetTimer(BulletTimer, this, &ABulletBase::DestroyHitEffect, 1.5f, false);
	Destroy();
}

void ABulletBase::AddSpeed(float newSpeed)
{
	Movement->InitialSpeed = newSpeed;
	Movement->MaxSpeed = newSpeed;
}



