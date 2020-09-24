// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Weapon/BulletDamageType.h"

// Sets default values
ABullet::ABullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = Sphere;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnBeginOverlap);
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABullet::ApplyDamageProcess()
{
	UE_LOG(LogClass, Warning, TEXT("ApplyDamage"));

	UGameplayStatics::ApplyPointDamage(PlayerOutHit.GetActor(), 1.0f, -PlayerOutHit.ImpactNormal, PlayerOutHit, PlayerController, this, UBulletDamageType::StaticClass());

}

void ABullet::SetDamageInfo(FHitResult OutHit, AController* Controller)
{
	PlayerOutHit = OutHit;
	PlayerController = Controller;
}

void ABullet::OnBeginOverlap(UPrimitiveComponent * OverlappedComponent,
	AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->ActorHasTag(TEXT("Player")))
	{
		ApplyDamageProcess();
	}
}

