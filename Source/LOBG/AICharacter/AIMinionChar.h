// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "../LOBGGameInstance.h"

#include "AIMinionChar.generated.h"

UENUM(BlueprintType)
enum class EMinioonState : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Chase UMETA(DisplayName = "Chase"),
	Battle UMETA(DisplayName = "Battle"),
	Dead UMETA(DisplayName = "Dead")
};

UCLASS()
class LOBG_API AAIMinionChar : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAIMinionChar();

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
		class UWeaponComponent* Weapon;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// AI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
		class UBehaviorTree* BTMinion;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		class UPawnSensingComponent* PawnSensing;
		
	UFUNCTION()
		void ProcessSeenPawn(APawn* Pawn);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Network
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// State
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float WalkSpeed = 150.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float MaxHP = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float CurrentHP = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float AttackDamage = 10.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		FName TeamName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		EMinioonState CurrentState;

	UFUNCTION(BlueprintCallable)
		void SetState(EMinioonState NewState);

	// Aim
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		float AimPitch = 0.f;

	// MoveTarget
	class AWaveCoursePoint* CurrentMoveTarget;

	// Fire
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Status")
		uint8 bIsFire : 1;
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AEmissiveBullet> ProjectileClass;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* FireMontage;

	void OnFire();

	//UFUNCTION(Server, Reliable)
	//	void Server_ProcessFire(FVector StartLine, FVector EndLine);
	//void Server_ProcessFire_Implementation(FVector StartLine, FVector EndLine);


};
