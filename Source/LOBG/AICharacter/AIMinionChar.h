// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	float WalkSpeed = 150.f;

	// AI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
		class UBehaviorTree* BTMinion;

	// State
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
		EMinioonState CurrentState;

	UFUNCTION(BlueprintCallable)
		void SetState(EMinioonState NewState);

	// MoveTarget
	class AWaveCoursePoint* CurrentMoveTarget;
};
