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
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class USkeletalMesh* RoboMeshes;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		TArray<class UMaterialInterface*> RoboMaterials;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
		class UStaticMeshComponent* TeamLampEye;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
		class UStaticMeshComponent* TeamLampBack;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		TArray<class UMaterialInterface*> TeamLampMaterials;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
		float PawnSensingSightRadius = 1500.f;

	// MoveTarget
	class AWaveCoursePoint* CurrentMoveTarget;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Data")
		int WaveCourse;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Network
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// State
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float WalkSpeed = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Data")
		float MaxHP = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = "OnRep_CurrentHP", Category = "Data")
		float CurrentHP = 100.f;
	UFUNCTION()
		void OnRep_CurrentHP();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float AttackDamage = 10.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = "OnRep_TeamName", Category = "Data")
		FName TeamName;
	UFUNCTION()
		void OnRep_TeamName();
	void SetTeamName(FName MyTeamName);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = "OnRep_CurrentState", Category = "Data")
		EMinioonState CurrentState;
	UFUNCTION()
		void OnRep_CurrentState();
	UFUNCTION(BlueprintCallable)
		void SetState(EMinioonState NewState);

	// Aim
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Status")
		float AimPitch = 0.f;

	// Fire
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Status")
		uint8 bIsFire : 1;
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AEmissiveBullet> ProjectileClass;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* FireMontage;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class USoundBase* FireSound;

	void OnFire(FVector TargetLocation);

	UFUNCTION(NetMulticast, Reliable)
		void NetMulticast_ProcessFire(FVector SpawnLocation, FRotator SpawnRotation);
	void NetMulticast_ProcessFire_Implementation(FVector SpawnLocation, FRotator SpawnRotation);

	//UPROPERTY()
	//	USoundAttenuation* AttenuationSettings;

	// TakeDamage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintImplementableEvent)
		void BP_HitEffect();

	// Death
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* DeathMontage;

	UFUNCTION(NetMulticast, Reliable)
		void NetMulticast_StartDeath(int Index);
	void NetMulticast_StartDeath_Implementation(int Index);

	void DeathSetting();		// 죽을 때의 설정들

	UFUNCTION(BlueprintCallable)
		bool Montage_IsPlaying(class UAnimMontage* AnimMontage);

	// UI
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UHUDBarSceneComponent* HPBarHUD;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UWidgetComponent* Widget;

	void InitHPBar();
	void UpdateHPBar();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		float MinionExp = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		int MinionMoney = 100;
};
