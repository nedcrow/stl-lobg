// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../LOBGGameInstance.h"
#include "../ChoiceMesh/MeshWidgetBase.h"
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

	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)		// 무기 구현 방법
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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Status")
	float WalkSpeed = 300.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Status")
	float RunSpeed = 600.f;

	void DoSprint();
	void UndoSprint();

	UFUNCTION(Server, Reliable)
		void Server_SetMaxWalkSpeed(float NewSpeed);
	void Server_SetMaxWalkSpeed_Implementation(float NewSpeed);	


#pragma region FireAndTakeDamage
	//충돌할 오브젝트 타입 에디터에서 설정
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
		TSubclassOf<class ABulletBase> BulletClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Bullet")
		uint8 bIsFire : 1;
	
	// 총알이 확산되는 각도. 기본 사격 상태에서의 수치.
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bullet")
	//	float RandFireAngle = 4.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* FireMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* FireMontage_Female;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* FireMontage_Male;

	class UAnimMontage* CurrentFireMontage;

	// Fire
	void StartFire();
	void StopFire();

	FTimerHandle BulletTimer;

	//실질적인 총알 발사 함수
	void OnFire();

	UFUNCTION(Server, Reliable)
		void Server_ProcessFire(FVector StartLine, FVector EndLine);
	void Server_ProcessFire_Implementation(FVector StartLine, FVector EndLine);
	UFUNCTION(NetMulticast, Reliable)
		void NetMulticast_ProcessFire(FVector SpawnLocation, FRotator SpawnRotation);
	void NetMulticast_ProcessFire_Implementation(FVector SpawnLocation, FRotator SpawnRotation);

	// TakeDamage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
#pragma endregion

	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = "OnRep_CurrentHP", Category = "Status")
		float CurrentHP = 100.f;

	UFUNCTION()
		void OnRep_CurrentHP();

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Status")
	UPROPERTY(Replicated)
		float MaxHP;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
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

	// AimOffset
	FRotator GetAimOffset();

	// Animations that FSMless
#pragma region FSMless Animations
	// Death
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* DeathMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* DeathMontage_Female;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* DeathMontage_Male;

		class UAnimMontage* CurrentDeathMontage;

	UFUNCTION(NetMulticast, Reliable)
		void NetMulticast_StartDeath(int Index);
	void NetMulticast_StartDeath_Implementation(int Index);

	// Reload
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* ReloadMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* ReloadMontage_Female;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* ReloadMontage_Male;

		class UAnimMontage* CurrentReloadMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Status")
		uint8 bIsReload : 1;

	UFUNCTION(Server, Reliable)
		void Server_SetReload(bool NewState);
	void Server_SetReload_Implementation(bool NewState);

	void StartReload();

	// Hit
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* HitActionMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* HitActionMontage_Female;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* HitActionMontage_Male;

		class UAnimMontage* CurrentHitActionMontage;

	UFUNCTION(NetMulticast, Reliable)
		void NetMulticast_StartHitMontage(int Number);
	void NetMulticast_StartHitMontage_Implementation(int Number);
#pragma endregion

#pragma region ReSpawn
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "State")
		EBattleCharacterState CurrentState;

	//게임모드에 리스폰 요청
	UFUNCTION(Server, Reliable)
		void Server_CallReSpawnToGM();
	void Server_CallReSpawnToGM_Implementation();

	//리스폰 시 UI 갱신
	UFUNCTION(NetMulticast, Reliable)
		void NetMulticast_ReSpawnUI();
	void NetMulticast_ReSpawnUI_Implementation();

	//죽을 때의 설정들
	void DeathSetting();
#pragma endregion

	//미니언을 죽이고 얻은 전리품 세팅
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

	//HPHUD 업데이트
	void UpdateHPBar();

	void SetHUDVisible();

	UFUNCTION(Server, Reliable)
	void Server_ItemAttack();
	void Server_ItemAttack_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_ItemSpeed();
	void Server_ItemSpeed_Implementation();

	void ItemSpeed();

	UFUNCTION(Server, Reliable)
	void Server_FullHP();
	void Server_FullHP_Implementation();


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MeshType")
		class USkeletalMesh* FemaleMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MeshType")
		TSubclassOf<class UAnimInstance> FemaleAnim;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MeshType")
		class USkeletalMesh* MaleMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MeshType")
		TSubclassOf<class UAnimInstance> MaleAnim;

	EMeshType PlayerMeshType = EMeshType::None;

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_SetMeshSettings(const EMeshType& MyMeshType);
	void NetMulticast_SetMeshSettings_Implementation(const EMeshType& MyMeshType);

	UFUNCTION(Server, Reliable)
		void Server_AngleDown();
	void Server_AngleDown_Implementation();

	UFUNCTION(Server, Reliable)
		void Server_SetPSFOV();
	void Server_SetPSFOV_Implementation();

	void SetPSFOV();

	void GoHome();

	UFUNCTION(Server, Reliable)
	void Server_GoHome();
	void Server_GoHome_Implementation();

	void SetPotionSlot();

	void EatPotion();

	UFUNCTION(Server, Reliable)
	void Server_ChangeGunMesh(class USkeletalMesh* GunMesh);
	void Server_ChangeGunMesh_Implementation(class USkeletalMesh* GunMesh);

	UFUNCTION(NetMulticast, Reliable)
		void NetMulticast_ChangeGunMesh(class USkeletalMesh* GunMesh);
	void NetMulticast_ChangeGunMesh_Implementation(class USkeletalMesh* GunMesh);

	void ChangeGunMesh(const FString& GunItemName);

	UFUNCTION(Server, Reliable)
		void Server_MaxHPUp();
	void Server_MaxHPUp_Implementation();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Status")
	float BulletSpeed = 4500.f;

	UFUNCTION(Server, Reliable)
		void Server_BulletSpeedUp();
	void Server_BulletSpeedUp_Implementation();

	FString CurrentGunName;

	void GunUpgrade();

	UFUNCTION(Server, Reliable)
		void Server_AttackUpdate(float AttackDamage);
	void Server_AttackUpdate_Implementation(float AttackDamage);

	void SwitchAttackpoint(int index);

	void PressCheatKey();
};
