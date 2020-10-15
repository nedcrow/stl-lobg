// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../LOBGGameInstance.h"
#include "BattleCharacter.generated.h"

UENUM(BlueprintType)
enum class EBattleCharacterState : uint8
{
	Normal = 0		UMETA(DisplayName = "Normal"),
	Battle = 1		UMETA(DisplayName = "Battle"),
	Dead = 2		UMETA(DisplayName = "Dead"),
};


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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)		// ���� ���� ���
		class UWeaponComponent* Weapon;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UHUDBarSceneComponent* HPBarHUD;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UWidgetComponent* Widget;

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



#pragma region FireAndTakeDamage
	//�浹�� ������Ʈ Ÿ�� �����Ϳ��� ����
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
		TSubclassOf<class ABulletBase> BulletClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Bullet")
		uint8 bIsFire : 1;

	FTimerHandle BulletTimer;

	void StartFire();
	void StopFire();

	//�������� �Ѿ� �߻� �Լ�
	void OnFire();

	UFUNCTION(Server, Reliable)
		void Server_ProcessFire(FVector StartLine, FVector EndLine);
	void Server_ProcessFire_Implementation(FVector StartLine, FVector EndLine);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
#pragma endregion

	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = "OnRep_CurrentHP", Category = "Status")
		float CurrentHP = 100.f;

	UFUNCTION()
		void OnRep_CurrentHP();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Status")
		float MaxHP = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Status")
		float AttackPoint = 5.f;

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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* ReloadMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Status")
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

#pragma region ReSpawn
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "State")
		EBattleCharacterState CurrentState;

	//���Ӹ�忡 ������ ��û
	UFUNCTION(Server, Reliable)
		void Server_CallReSpawnToGM();
	void Server_CallReSpawnToGM_Implementation();

	//������ �� UI ����
	UFUNCTION(NetMulticast, Reliable)
		void NetMulticast_ReSpawnUI();
	void NetMulticast_ReSpawnUI_Implementation();

	//���� ���� ������
	void DeathSetting();
#pragma endregion

	//�̴Ͼ��� ���̰� ���� ����ǰ ����
	UFUNCTION(Server, Reliable)
		void Server_SetBooty(int Money, float Exp);
	void Server_SetBooty_Implementation(int Money, float Exp);

	void SetBooty(int Money, float Exp);

	UFUNCTION(NetMulticast, Reliable)
		void NetMulticast_AddTag(const FName& PlayerTag);
	void NetMulticast_AddTag_Implementation(const FName& PlayerTag);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		FName TeamName;

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_InitHPBar(ETeamColor color);
	void NetMulticast_InitHPBar_Implementation(ETeamColor color);

	//HPHUD ������Ʈ
	void UpdateHPBar();

	void SetHUDVisible();

	UFUNCTION(Server, Reliable)
	void Server_ItemAttack();
	void Server_ItemAttack_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_ItemSpeed();
	void Server_ItemSpeed_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_ItemHP();
	void Server_ItemHP_Implementation();
};
