// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BattleCharacter.generated.h"

UCLASS()
class LOBG_API ABattleCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABattleCharacter();

	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UCameraComponent* Camera;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)		// 무기 구현 방법
	//	class UTPSWeaponComponent* Weapon;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// For RPC
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Move
	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookUp(float Value);
	void Turn(float Value);

	// Sprint
	float WalkSpeed = 300.f;
	float RunSpeed = 600.f;

	void DoSprint();
	void UndoSprint();

	UFUNCTION(Server, Reliable)
		void Server_SetMaxWalkSpeed(float NewSpeed);
		void Server_SetMaxWalkSpeed_Implementation(float NewSpeed);



	//BattlePC에서 입력받아 호출되는 총알 발사 함수
	void OnFire();

	UFUNCTION(Server, Reliable)
		void Server_ProcessFire(FVector StartLine, FVector EndLine);
		void Server_ProcessFire_Implementation(FVector StartLine, FVector EndLine);

	//충돌할 오브젝트 타입 에디터에서 설정
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
		TSubclassOf<class ABulletBase> BulletClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
		class USceneComponent* Muzzle;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

	// Ironsight
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Status")
		uint8 bIsIronsight : 1;

	UFUNCTION(Server, Reliable)
		void Server_SetIronsight(bool State);
		void Server_SetIronsight_Implementation(bool State);

	void StartIronsight();
	void StopIronsight();

	// Crouch
	void StartCrouch();

	// Lean
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Status")
		uint8 bIsLeanLeft : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Status")
		uint8 bIsLeanRight : 1;

	UFUNCTION(Server, Reliable)
		void Server_SetLeanLeft(bool State);
		void Server_SetLeanLeft_Implementation(bool State);

	UFUNCTION(Server, Reliable)
		void Server_SetLeanRight(bool State);
		void Server_SetLeanRight_Implementation(bool State);

	void StartLeanLeft();
	void StopLeanLeft();
	void StartLeanRight();
	void StopLeanRight();

	// Animations that FSMless
#pragma region FSMless Animations
	// Death
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* DeathMontage;

	UFUNCTION(NetMulticast, Reliable)
		void NetMulticast_StartDeath(int Index);
		void NetMulticast_StartDeath_Implementation(int Index);

	// Reload
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Data")
		class UAnimMontage* ReloadMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bIsReload : 1;

	UFUNCTION(Server, Reliable)
		void Server_SetReload(bool NewState);
		void Server_SetReload_Implementation(bool NewState);

		void StartReload();

	// Hit
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* HitActionMontage;

	UFUNCTION(NetMulticast, Reliable)
		void NetMulticast_StartHitMontage(int Number);
		void NetMulticast_StartHitMontage_Implementation(int Number);
#pragma endregion

};
