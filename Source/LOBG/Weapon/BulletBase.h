// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletBase.generated.h"

UCLASS()
class LOBG_API ABulletBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABulletBase();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UStaticMeshComponent* StaticMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class USphereComponent* Sphere;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UProjectileMovementComponent* Movement;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Damage
	//EApplyDamageType CurrentDamageType;
	FHitResult TraceHit;
	AController* SummonerController;
	float AttackPoint = 0.f;
	float AttackRadial = 0.f;
	void SetDamageInfo(AController* Controller, float NewAttackPoint, float NewAttackRadial, FName NewTeamName);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
		TSubclassOf<class UBulletDamageType> DamageType;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
	FTimerHandle BulletTimer;

	// Effect
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
	class UParticleSystem* HitEffect;
	class UParticleSystemComponent* HitEffectComponent;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
	FVector HitEffectScale = FVector::OneVector;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
	class USoundBase* FireSound;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
	class USoundBase* HitSound;

	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticast_HitEffect(FVector SpawnLocation, FVector HitRirection);
	void NetMulticast_HitEffect_Implementation(FVector SpawnLocation, FVector HitDirection);

	void DestroyHitEffect();

	// ����Ʈ �ߺ� ���� ����.
	// bulletbase�� ���������� ���������� �ǰ� �־ �������� ���̷�Ż�޽��� ������ ������ �Լ��� �����ϴ� ������ �������� �߰�.
	bool bCanHit = true;

	// OverlapEvent
	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	// HitEvent
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Team
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	FName TeamName;

	//float LifeSpanTime = 0.6f;

	void AddSpeed(float newSpeed);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ExposeOnSpawn = true), Category = "Data")
	float MyBulletSpeed;

};
