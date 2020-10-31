// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "GunDataTable.generated.h"

USTRUCT(BlueprintType)
struct LOBG_API FGunData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString GunName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TAssetPtr<class UMaterialInstance> GunImage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TAssetPtr<class USkeletalMesh> GunMesh;

};

UCLASS()
class LOBG_API AGunDataTable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGunDataTable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
