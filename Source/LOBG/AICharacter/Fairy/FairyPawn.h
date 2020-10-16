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

	// Store
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	// Base property
	UPROPERTY(BlueprintReadWrite, Replicated, EditAnywhere, Category = "Status")
	float MaxHP = 100;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = "OnRepCurrentHP", EditAnywhere, Category = "Status")
	float CurrentHP = 100;;

	UPROPERTY(BlueprintReadWrite, Replicated, EditAnywhere, Category = "Status")
	float AttackPoint = 11;

	UFUNCTION()
	void OnRepCurrentHP();


	// State & AI ref - 변할 수 있는 상태
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
	ETeamColor TeamColor=ETeamColor::None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
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
	class UParticleSystem* SpawnEffect;

	UFUNCTION()
	void StartFireTo(FVector TargetLocation);

	UFUNCTION(Server, Reliable)
	void Server_ProcessFire(FVector StartLocation, FRotator StartDirection, FVector TargetLocation);
	void Server_ProcessFire_Implementation(FVector StartLocation, FRotator StartDirection, FVector TargetLocation);
	
	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticast_SpawnEffect(FVector SpawnLocation);
	void NetMulticast_SpawnEffect_Implementation(FVector SpawnLocation);

	/* Second */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
	float ReloadingTime = 12;
	float ReloadingPercentage = 0;

	UFUNCTION()
	bool CallReload();
	void Reload();
	void ReloadAnimation();
	int CurrentBulletCount;
	uint8 bIsCasting:1;
	FTimerHandle BulletTimer;

	// HUD
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UHUDBarSceneComponent* HPBarHUD;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UWidgetComponent* Widget;

	UFUNCTION()
	void UpdateHPBar();

	// Reset Tag (최초에는 BP에서 선택한 값으로 세팅) & Team
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_ResetTags(const FName& TeamTag);
	void NetMulticast_ResetTags_Implementation(const FName& TeamTag);
	
	UFUNCTION()
	FName GetTeamName(APawn * Pawn);
	FName TeamName;
};
