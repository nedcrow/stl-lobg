// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TempMinion.generated.h"

UCLASS()
class LOBG_API ATempMinion : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATempMinion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UStaticMeshComponent* Mesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class USphereComponent* Sphere;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	float MaxHP = 200.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = "OnRep_CurrentHP", Category = "Status")
	float CurrentHP = 200.f;

	UFUNCTION()
		void OnRep_CurrentHP();

	int MinionMoney = 100;

	float MinionExp = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UHUDBarSceneComponent* HPBarHUD;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UWidgetComponent* Widget;

	void UpdateHPBar();
};
