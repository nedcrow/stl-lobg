// Fill out your copyright notice in the Description page of Project Settings.


#include "MissileHubComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values for this component's properties
UMissileHubComponent::UMissileHubComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	for (int i = 0; i < LimmitCount; i++) {
		//�����κ�
		//FString::Printf�� MissileName�� for�� ���� ����� Missile ������ �Ҵ�
		FString MissileName = FString::Printf(TEXT("Missile_%d"), i);
		UStaticMeshComponent* Missile = CreateDefaultSubobject<UStaticMeshComponent>(FName(*MissileName));
		Missile->SetStaticMesh(MissileMesh);
		MissileMeshes.Add(Missile);
	}
}


// Called when the game starts
void UMissileHubComponent::BeginPlay()
{
	Super::BeginPlay();

	/*for (auto iter = MissileMeshes.begin(); iter != MissileMeshes.end(); ++iter) {

	}*/
	
}


// Called every frame
void UMissileHubComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

