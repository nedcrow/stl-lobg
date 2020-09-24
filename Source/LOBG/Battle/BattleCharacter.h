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

	// ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UCameraComponent* Camera;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)		// ���� ���� ���
	//	class UTPSWeaponComponent* Weapon;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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



	//BattlePC���� �Է¹޾� ȣ��Ǵ� �Ѿ� �߻� �Լ�
	void OnFire();

	UFUNCTION(Server, Reliable)
		void Server_ProcessFire(FVector StartLine, FVector EndLine);
		void Server_ProcessFire_Implementation(FVector StartLine, FVector EndLine);

	//�浹�� ������Ʈ Ÿ�� �����Ϳ��� ����
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
		TSubclassOf<class ABullet> BulletClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
		class USceneComponent* Muzzle;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

	// Ironsight
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bIsIronsight : 1;

	void StartIronsight();
	void StopIronsight();

	UFUNCTION(Server, Reliable)
		void C2S_SetIronsight(bool State);
		void C2S_SetIronsight_Implementation(bool State);

	// Crouch
	void StartCrouch();

	// Reload
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bIsReload : 1;

	UFUNCTION(Server, Reliable)
		void C2S_SetReload(bool newState);
		void C2S_SetReload_Implementation(bool newState);

	// Lean
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bIsLeanLeft : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bIsLeanRight : 1;

	//�ִϸ��̼ǿ��� �� ���º���

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* DeadMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* ReloadMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* HitActionMontage;

};
