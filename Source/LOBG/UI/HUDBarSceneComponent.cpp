// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDBarSceneComponent.h"
#include "../Battle/BattleCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UHUDBarSceneComponent::UHUDBarSceneComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHUDBarSceneComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHUDBarSceneComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	APlayerCameraManager* CM = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (CM)
	{
		SetWorldRotation(CM->GetCameraRotation());
	}
}

