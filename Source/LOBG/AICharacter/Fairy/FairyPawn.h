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
	class AMeshesRing* MissileHub;

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
	EFairyState CurrentState;

	UFUNCTION(BlueprintCallable)
	void SetCurrentState(EFairyState NewState);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UBehaviorTree* BTFairy;

	// Sensing
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UPawnSensingComponent* PawnSensingComponent;

	// process after saw something
	UFUNCTION()
	void ProcessSeenPawn(APawn* Pawn);

	// Base property
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		TArray<UStaticMeshComponent*> Meshes;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UStaticMesh* Mesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int MeshCount;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Speed = 100.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Radius = 32.f;

	// About rotate
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		uint8 bIsRotatable : 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		uint8 bIsLookBody : 1;
};
