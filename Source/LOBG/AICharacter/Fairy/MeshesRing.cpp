// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshesRing.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
AMeshesRing::AMeshesRing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));

	InnerCore = CreateDefaultSubobject<USceneComponent>(TEXT("InnerCore"));
	InnerCore->SetupAttachment(RootComponent);

	IncetancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>("IncetancedMesh");
	IncetancedMesh->SetupAttachment(InnerCore);
}

// Called when the game starts or when spawned
void AMeshesRing::BeginPlay()
{
	Super::BeginPlay();

	//AddComponent()

	// StaticMeshes creating & Transform setting.
	float RUnit = 360.f / MeshCount;
	UE_LOG(LogClass, Warning, TEXT("Radius: %f / Unit: %f"), Radius, RUnit);
	for (int i = 0; i < MeshCount; i++) {
		FName MeshName = *FString::Printf(TEXT("Missile_%d"), i); // CreateDefaultSubobject 매개변수 고려하여 FString::Printf로
		UE_LOG(LogClass, Warning, TEXT("Missile_%d"), i);
		FVector TempPosition = FVector(Radius, 0, 0);
		FVector TempRotation = bIsRotatable ? FVector(0, RUnit * i, 0) : FVector::ZeroVector;
		FRotator TempRotation2 = bIsRotatable ? FRotator(0, RUnit * i, 0) : FRotator::ZeroRotator;

		float rad = 0.0174532925199444;
		FQuat qx(FVector(1, 0, 0), -TempRotation.X * rad);
		FQuat qz(FVector(0, 0, 1), -TempRotation.Y * rad);
		FQuat qy(FVector(0, 1, 0), -TempRotation.Z * rad);

		FQuat qu = qy * qz * qx; // Change Rotation Order if necessary
		FRotator Rotation(qu);
		UE_LOG(LogClass, Warning, TEXT("FRotator_(%f, %f, %f)"), Rotation.Roll, Rotation.Pitch, Rotation.Yaw);

		IncetancedMesh->AddInstance(
			FTransform(TempRotation2, TempPosition, FVector::OneVector)
		);
		IncetancedMesh->AddRelativeRotation(FRotator(0, RUnit * i, 0));
		UE_LOG(LogClass, Warning, TEXT("Meshes_Count: %d"), IncetancedMesh->GetInstanceCount());
	}
}

// Called every frame
void AMeshesRing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsRotatable) RotateAround(DeltaTime);
}

void AMeshesRing::RotateAround(float DeltaTime)
{
	FRotator TempRotation = FRotator(0, DeltaTime * Speed, 0);
	InnerCore->AddRelativeRotation(TempRotation);
}