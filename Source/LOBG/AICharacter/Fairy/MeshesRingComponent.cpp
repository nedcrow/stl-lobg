// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshesRingComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values for this component's properties
UMeshesRingComponent::UMeshesRingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<USceneComponent>(TEXT("Body"));

	//float���̶� 360.f�� �������Ѵ�.
	float unit = 360.f / LimmitCount;

	for (int i = 0; i < LimmitCount; i++) {
		//�����κ�
		//FString::Printf�� MissileName�� for�� ���� ����� TempMesh ������ �Ҵ�
		FString MissileName = FString::Printf(TEXT("Missile_%d"), i);
		UStaticMeshComponent* TempMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName(*MissileName));
		TempMesh->SetStaticMesh(Mesh);
		TempMesh->SetupAttachment(Body);
		TempMesh->SetRelativeLocation(FVector(Radius, 0, 0));
		TempMesh->SetRelativeRotation(FRotator(0, unit*i, 0));
		Meshes.Add(TempMesh);
	}
}


// Called when the game starts
void UMeshesRingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMeshesRingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FRotator TempRotation = FRotator(
		bIsPitch ? DeltaTime * Speed : 0,
		bIsYaw ? DeltaTime * Speed : 0,
		bIsRoll ? DeltaTime * Speed : 0
	);
	Body->AddRelativeRotation(TempRotation);
}

