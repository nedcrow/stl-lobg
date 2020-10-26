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
	PrimaryActorTick.bCanEverTick = false;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = Sphere;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);
	StaticMesh->bCastDynamicShadow = false;		// Disallow mesh to cast dynamic shadows
	StaticMesh->CastShadow = false;			// Disallow mesh to cast other shadows

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

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

void ABulletBase::SetDamageInfo(AController* Controller, float NewAttackPoint, float NewAttackRadial, FName NewTeamName)
{
	//TraceHit = OutHit;
	SummonerController = Controller;
	AttackPoint = NewAttackPoint;
	AttackRadial = NewAttackRadial;
	TeamName = NewTeamName;
	Tags.Add(TeamName);
}

void ABulletBase::NetMulticast_HitEffect_Implementation(FVector SpawnLocation, FVector HitDirection)
{
	if (HitEffect)
	{
		FRotator Rotation = HitDirection.Rotation();
		UParticleSystemComponent* Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitEffect,
			SpawnLocation,
			Rotation,
			HitEffectScale
		);
	}

	// 클라이언트 미니언 피격 이펙트용 가짜 데미지.
	if (!GetWorld()->IsServer() && AttackRadial > 0.f)
	{
		UGameplayStatics::ApplyRadialDamage(
			GetWorld(),
			AttackPoint,
			SpawnLocation,
			200.f,
			UBulletDamageType::StaticClass(),
			TArray<AActor*>(),
			this,
			SummonerController,
			true,
			ECC_Visibility);
	}
}

//플레이어에 충돌하면
void ABulletBase::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogClass, Warning, TEXT("Other Actor : %s %s"), *OtherActor->GetName(), *SweepResult.BoneName.ToString());

	// 같은팀이라면 return
	if (OtherActor == NULL || OtherActor->ActorHasTag(TeamName))
	{
		//NetMulticast_HitEffect(OtherActor->GetActorLocation());
		Destroy();
		return;
	}
	else if (SummonerController && OtherActor == SummonerController->GetPawn())
	{
		return;
	}
	else if (!GetWorld()->IsServer() && OtherActor->ActorHasTag(TEXT("Minion")))
	{
		// 클라이언트 미니언 피격 이펙트용 가짜 데미지.
		UGameplayStatics::ApplyDamage(OtherActor, AttackPoint, SummonerController, this, UBulletDamageType::StaticClass());
	}

	// ApplyDamage
	//bool RadialType = AttackRadial > 0 ? true : false;

	if (GetWorld()->IsServer())		// 데미지는 서버에서만 계산한다.
	{
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

			NetMulticast_HitEffect(SweepResult.ImpactPoint + -GetVelocity().GetSafeNormal() * 20.f, -SweepResult.ImpactNormal);
		}
		else if (OtherActor->ActorHasTag(TEXT("Player")))
		{
			UGameplayStatics::ApplyPointDamage(OtherActor, AttackPoint, -SweepResult.ImpactNormal, SweepResult, SummonerController, this, UBulletDamageType::StaticClass());

			NetMulticast_HitEffect(SweepResult.ImpactPoint + -GetVelocity().GetSafeNormal() * 20.f, -SweepResult.ImpactNormal);
			Destroy();
		}
		else if (OtherActor->ActorHasTag(TEXT("Minion")))
		{
			UGameplayStatics::ApplyPointDamage(OtherActor, AttackPoint, -SweepResult.ImpactNormal, SweepResult, SummonerController, this, UBulletDamageType::StaticClass());

			NetMulticast_HitEffect(SweepResult.ImpactPoint + -GetVelocity().GetSafeNormal() * 20.f, -SweepResult.ImpactNormal);
			Destroy();
		}
		else if (OtherActor->ActorHasTag(TEXT("Tower")))
		{
			UGameplayStatics::ApplyDamage(OtherActor, AttackPoint, SummonerController, this, UBulletDamageType::StaticClass());
			NetMulticast_HitEffect(SweepResult.ImpactPoint + -GetVelocity().GetSafeNormal() * 20.f, -SweepResult.ImpactNormal);
			Destroy();
		}
	}

	//맞은게 플레이어가 아닌 다른 액터라면 사라진다.
	//스스로 사라지지 않도록 Bullet 태그를 검사한다.
	else if (!OtherActor->ActorHasTag(TEXT("Bullet")) && !OtherComp->ComponentHasTag(TEXT("Weapon")))
	{
		UE_LOG(LogClass, Warning, TEXT("Other Actor : %s"), *OtherActor->GetName());
		NetMulticast_HitEffect(OtherActor->GetActorLocation(), -SweepResult.ImpactNormal);
		Destroy();
	}
}

void ABulletBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 같은팀이라면 return
	if (OtherActor == NULL || OtherActor->ActorHasTag(TeamName))
	{
		//NetMulticast_HitEffect(OtherActor->GetActorLocation());
		Destroy();
		return;
	}
	else if (SummonerController && OtherActor == SummonerController->GetPawn())
	{
		return;
	}
	else if (!GetWorld()->IsServer() && OtherActor->ActorHasTag(TEXT("Minion")))
	{
		// 클라이언트 미니언 피격 이펙트용 가짜 데미지.
		UGameplayStatics::ApplyDamage(OtherActor, AttackPoint, SummonerController, this, UBulletDamageType::StaticClass());
	}

	// ApplyDamage
	//bool RadialType = AttackRadial > 0 ? true : false;

	if (GetWorld()->IsServer())		// 데미지는 서버에서만 계산한다.
	{
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
				ECC_Visibility);
		}


		//else if (OtherActor->ActorHasTag(TEXT("Player")))
		//{
		//	UGameplayStatics::ApplyPointDamage(OtherActor, AttackPoint, -Hit.ImpactNormal, Hit, SummonerController, this, UBulletDamageType::StaticClass());

		//	NetMulticast_HitEffect(OtherActor->GetActorLocation(), -Hit.ImpactNormal);
		//}
		//else if (OtherActor->ActorHasTag(TEXT("Minion")))
		//{
		//	UGameplayStatics::ApplyPointDamage(OtherActor, AttackPoint, -Hit.ImpactNormal, Hit, SummonerController, this, UBulletDamageType::StaticClass());

		//	NetMulticast_HitEffect(OtherActor->GetActorLocation(), -Hit.ImpactNormal);
		//}
		//else if (OtherActor->ActorHasTag(TEXT("Tower")))
		//{
		//	UGameplayStatics::ApplyDamage(OtherActor, AttackPoint, SummonerController, this, UBulletDamageType::StaticClass());
		//	NetMulticast_HitEffect(OtherActor->GetActorLocation(), -Hit.ImpactNormal);
		//}
	}

	NetMulticast_HitEffect(Hit.ImpactPoint + Hit.ImpactNormal * 20.f, -Hit.ImpactNormal);


	// Sound
	// Effect Multicast_SpawnHitEffectAndDecal(OutHit);
	Destroy();
}

