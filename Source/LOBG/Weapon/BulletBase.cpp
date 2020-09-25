// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Weapon/BulletDamageType.h"

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

void ABulletBase::ApplyDamageProcess()
{
	UE_LOG(LogClass, Warning, TEXT("ApplyDamage"));

	UGameplayStatics::ApplyPointDamage(PlayerOutHit.GetActor(), 10.0f, -PlayerOutHit.ImpactNormal, PlayerOutHit, PlayerController, this, UBulletDamageType::StaticClass());

	//����

	//����Ʈ

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
	//�÷��̾ �浹�ϸ�
	if (OtherActor->ActorHasTag(TEXT("Player")))
	{
		//������ ����
		ApplyDamageProcess();
	}

	//������ �÷��̾ �ƴ� �ٸ� ���Ͷ�� �������.
	//������ ������� �ʵ��� Bullet�±׸� �˻��Ѵ�.
	else if(!OtherActor->ActorHasTag(TEXT("Bullet")))
	{
		UE_LOG(LogClass, Warning, TEXT("Other Actor : %s"), *OtherActor->GetName());
		Destroy();
	}
}

