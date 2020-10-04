// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FairyPawn.generated.h"

UENUM()
enum class EFairyState : uint8 {
	Idle = 0 UMETA(Display = Idle),
	Fight = 1 UMETA(Display = Fight),
	Death = 2 UMETA(Display = Death),
};

UENUM()
enum class ETeam : uint8 {
	TeamBlue = 0 UMETA(Display = TeamBlue),
	TeamRed = 1 UMETA(Display = TeamRed),
	Neutral = 2 UMETA(Display = Neutral),
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AMeshesRing")
	class UMeshesRingComponent* MeshesRingComponent;

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
	UPROPERTY(BlueprintReadWrite, Replicated, EditAnywhere)
	float MaxHP;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = "OnRepCurrentHP", EditAnywhere)
	float CurrentHP;

	UFUNCTION()
	void OnRepCurrentHP();


	// State & AI ref
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
	ETeam CurrentTeam;

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
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 bIsEndFire : 1;

	UFUNCTION()
	void StartFireTo(FVector TargetLocation);

	UFUNCTION()
	void EndFire();
};
