// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "../../LOBGGameInstance.h"

#include "FairyPawn.generated.h"

UENUM()
enum class EFairyState : uint8 {
	Idle = 0 UMETA(Display = Idle),
	Fight = 1 UMETA(Display = Fight),
	Death = 2 UMETA(Display = Death),
};


UCLASS()
class LOBG_API AFairyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AFairyPawn();

	// Base component
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class USphereComponent* SphereCollision;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UStaticMeshComponent* Body;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class USkeletalMeshComponent* Head;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AMeshesRing")
	class UMeshesRingComponent* ActiveMeshesRingComp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AMeshesRing")
	class UMeshesRingComponent* RestMeshesRingComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Damage
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Date_Sound")
	USoundBase* HitSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Sound")
	USoundBase* DestroySound;

	// Store
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	// Base property
	UPROPERTY(BlueprintReadWrite, Replicated, EditAnywhere, Category = "Data")
	uint8 bIsBoss:1;

	UPROPERTY(BlueprintReadWrite, Replicated, EditAnywhere, Category = "Data")
	float MaxHP = 100;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = "OnRepCurrentHP", EditAnywhere, Category = "Data")
	float CurrentHP = 100;;

	UPROPERTY(BlueprintReadWrite, Replicated, EditAnywhere, Category = "Data")
	float AttackPoint = 25;

	UPROPERTY(BlueprintReadWrite, Replicated, EditAnywhere, Category = "Data")
	float FairyMoney = 1000;

	UPROPERTY(BlueprintReadWrite, Replicated, EditAnywhere, Category = "Data")
	float FairyExp = 1000;


	UFUNCTION()
	void OnRepCurrentHP();

	UFUNCTION(Server, Reliable)
	void Server_CallRotationRingComponent();
	void Server_CallRotationRingComponent_Implementation();

	// State & AI ref - 변할 수 있는 상태
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	ETeamColor TeamColor=ETeamColor::None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	EFairyState CurrentState;

	UFUNCTION(BlueprintCallable)
	void SetCurrentState(EFairyState NewState);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UBehaviorTree* BTFairy;


	// Sensing
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UPawnSensingComponent* PawnSensingComponent;

	UFUNCTION()
	void ProcessSeenPawn(APawn* Pawn);


	// fire
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
	TSubclassOf<class ABulletBase> BulletClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
	float AttackRadial = 500;

	int CurrentMissileIndex = 0;

	UFUNCTION()
	void StartFireTo(FVector TargetLocation);

	UFUNCTION(Server, Reliable)
	void Server_ProcessFire(FVector StartLocation, FRotator StartDirection, FVector TargetLocation);
	void Server_ProcessFire_Implementation(FVector StartLocation, FRotator StartDirection, FVector TargetLocation);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_RemoveMissile();
	void NetMulticast_RemoveMissile_Implementation();

	void SetNextMissileIndex();

	// Reload
	/* Second */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
	float ReloadingTime = 10;
	TArray<float> ReloadingPercentages;

	uint8 bIsCasting : 1;
	FTimerHandle BulletTimer;

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_CallReloadAnimation();
	void NetMulticast_CallReloadAnimation_Implementation();
	void ReloadAnimation();
	void AddMissile(FTransform NewTransform);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Sound")
	USoundBase* ReloadSound;

	// Repair
	float RepairPerSec = 10.f;

	UFUNCTION()
	void Repair();


	// Effect
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UParticleSystem* HeadEffect;
	class UParticleSystemComponent* HeadEffectComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
	class UParticleSystem* SpawnEffect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
	class UParticleSystem* FireEffect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UParticleSystem* DeathEffect;

	//UFUNCTION(NetMulticast, Unreliable)
	//void NetMulticast_HeadEffect();
	//void NetMulticast_HeadEffect_Implementation();
	void SpawnHeadEffect();

	void SpawnSpawnEffect(FVector SpawnLocation);

	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticast_FireEffect(FVector SpawnLocation);
	void NetMulticast_FireEffect_Implementation(FVector SpawnLocation);

	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticast_DeadthEffect();
	void NetMulticast_DeadthEffect_Implementation();

	// HUD
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UHUDBarSceneComponent* HPBarHUD;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UWidgetComponent* Widget;

	UFUNCTION()
	void UpdateHPBar();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_InitHPBar(ETeamColor color);
	void NetMulticast_InitHPBar_Implementation(ETeamColor color);


	// Reset Tag (최초에는 BP에서 선택한 값으로 세팅) & Team
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_ResetTags(const FName& TeamTag);
	void NetMulticast_ResetTags_Implementation(const FName& TeamTag);
	
	FName TeamName;
};
