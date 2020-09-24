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

	//충돌할 오브젝트 타입 에디터에서 설정
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;


	//애니메이션에서 쓸 상태변수
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bIsSprint : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bIsCrouch : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bIsIronsight : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bIsLeanLeft : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bIsLeanRight : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bIsReload : 1;
};
