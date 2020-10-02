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

	// InstanceMeshes creating & Transform setting.
	float RUnit = 360.f / MeshCount;
	UE_LOG(LogClass, Warning, TEXT("Radius: %f / Unit: %f"), Radius, RUnit);
	for (int i = 0; i < MeshCount; i++) {
		FName MeshName = *FString::Printf(TEXT("Missile_%d"), i); // CreateDefaultSubobject 매개변수 고려하여 FString::Printf로
		float RotateDegree = RUnit * i;
		float x = Radius * (FMath::Cos(RotateDegree * OneDegree));
		float y = Radius * (FMath::Sin(RotateDegree * OneDegree));
		FVector Position = FVector(x, y, 0);
		FRotator Rotation = bIsLookBody ? FRotator(0, RotateDegree, 0) : FRotator::ZeroRotator;
		FVector Scale = FVector::OneVector;
		UE_LOG(LogClass, Warning, TEXT("Missile_%d: FRotator_(%f, %f, %f)"), i, Position.X, Position.Y, Position.Z);
		UE_LOG(LogClass, Warning, TEXT("Missile_%d: FRotator_(%f, %f, %f)"), i, Rotation.Roll, Rotation.Pitch, Rotation.Yaw);

		IncetancedMesh->AddInstance(FTransform(Rotation, Position, Scale));
		IncetancedMesh->AddRelativeRotation(FRotator(0, RotateDegree, 0));
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