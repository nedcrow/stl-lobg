// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReSpawn.generated.h"

UCLASS()
class LOBG_API AReSpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AReSpawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UBoxComponent* Box;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class USceneComponent* ReSpawnArea;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UStaticMeshComponent* Body;


	UFUNCTION()
		void BeginOverlapProcess(UPrimitiveComponent* OverlappedComponent, 
			AActor* OtherActor, 
			UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex, 
			bool bFromSweep, 
			const FHitResult & SweepResult);

	UFUNCTION()
		void EndOverlapProcess(UPrimitiveComponent* OverlappedComponent, 
			AActor* OtherActor, 
			UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex);
};
