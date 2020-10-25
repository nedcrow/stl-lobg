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
	InitialLifeSpan = 5.0f;

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

void ABulletBase::NetMulticast_HitEffect_Implementation(FVector SpawnLocation)
{
	if (HitEffect)
	{
		UParticleSystemComponent* Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitEffect,
			SpawnLocation
		);
	}
}

//�÷��̾ �浹�ϸ�
void ABulletBase::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogClass, Warning, TEXT("Other Actor : %s %s"), *OtherActor->GetName(), *SweepResult.BoneName.ToString());

	// �������̶�� return
	if (OtherActor == NULL || OtherActor->ActorHasTag(TeamName))
	{
		//NetMulticast_HitEffect(OtherActor->GetActorLocation());
		Destroy();
		return;
	}
	else if (!GetWorld()->IsServer() && OtherActor->ActorHasTag(TEXT("Minion")))
	{
		// Ŭ���̾�Ʈ �ǰ� ����Ʈ�� ��¥ ������.
		UGameplayStatics::ApplyDamage(OtherActor, AttackPoint, SummonerController, this, UBulletDamageType::StaticClass());
	}

	// �������� ���������� ����Ѵ�.
	
	if (GetWorld()->IsServer())
	{
		// ApplyDamage
		bool RadialType = AttackRadial > 0 ? true : false;
		if (OtherActor->ActorHasTag(TEXT("Player")))
		{
			if (RadialType) {
				UGameplayStatics::ApplyRadialDamage(
					GetWorld(),
					AttackPoint,
					OtherActor->GetActorLocation(),
					AttackRadial,
					UBulletDamageType::StaticClass(),
					TArray<AActor*>(),
					this,
					SummonerController,
					false,
					ECC_Visibility
				);
			}
			else {
				UGameplayStatics::ApplyPointDamage(OtherActor, AttackPoint, -SweepResult.ImpactNormal, SweepResult, SummonerController, this, UBulletDamageType::StaticClass());
			}
			NetMulticast_HitEffect(OtherActor->GetActorLocation());
			Destroy();
		}
		else if (OtherActor->ActorHasTag(TEXT("Minion")))
		{
			if (RadialType) {
				UGameplayStatics::ApplyRadialDamage(
					GetWorld(),
					AttackPoint,
					OtherActor->GetActorLocation(),
					AttackRadial,
					UBulletDamageType::StaticClass(),
					TArray<AActor*>(),
					this,
					SummonerController,
					false,
					ECC_Visibility
				);
				UE_LOG(LogTemp, Warning, TEXT("Minion apply radial damage"), AttackPoint);
			}
			else {
				UGameplayStatics::ApplyPointDamage(OtherActor, AttackPoint, -SweepResult.ImpactNormal, SweepResult, SummonerController, this, UBulletDamageType::StaticClass());
			}
			NetMulticast_HitEffect(OtherActor->GetActorLocation());
			Destroy();
		}
		else if (OtherActor->ActorHasTag(TEXT("Tower")))
		{
			UGameplayStatics::ApplyDamage(OtherActor, AttackPoint, SummonerController, this, UBulletDamageType::StaticClass());
			NetMulticast_HitEffect(OtherActor->GetActorLocation());
			Destroy();
		}
	}

	//������ �÷��̾ �ƴ� �ٸ� ���Ͷ�� �������.
	//������ ������� �ʵ��� Bullet �±׸� �˻��Ѵ�.
	else if (!OtherActor->ActorHasTag(TEXT("Bullet")) && !OtherComp->ComponentHasTag(TEXT("Weapon")))
	{
		UE_LOG(LogClass, Warning, TEXT("Other Actor : %s"), *OtherActor->GetName());
		NetMulticast_HitEffect(OtherActor->GetActorLocation());
		Destroy();
	}
}

void ABulletBase::OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	// Sound
	// Effect Multicast_SpawnHitEffectAndDecal(OutHit);
	Destroy();
}

