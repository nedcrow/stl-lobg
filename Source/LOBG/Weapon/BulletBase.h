// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletBase.generated.h"

//UENUM(BlueprintType)
//enum class EApplyDamageType : uint8
//{
//	Player		= 0 UMETA(DisplayName = "Player"),
//	Minion		= 1 UMETA(DisplayName = "Minion"),
//	Tower		= 2 UMETA(DisplayName = "Tower"),
//};

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
	void SetDamageInfo(AController* Controller, float NewAttackPoint, FName NewTeamName);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
		TSubclassOf<class UBulletDamageType> DamageType;

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
};
