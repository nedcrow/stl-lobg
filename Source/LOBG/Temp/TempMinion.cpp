// Fill out your copyright notice in the Description page of Project Settings.


#include "TempMinion.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Battle/BattlePC.h"
#include "../Battle/BattleCharacter.h"
#include "../UI/HPBarWidgetBase.h"
#include "../UI/HUDBarSceneComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATempMinion::ATempMinion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = Sphere;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	HPBarHUD = CreateDefaultSubobject<UHUDBarSceneComponent>(TEXT("HPBarHUD"));
	HPBarHUD->SetupAttachment(RootComponent);
	HPBarHUD->SetRelativeLocation(FVector(0, 0, 110));

	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->SetupAttachment(HPBarHUD);
	Widget->SetRelativeRotation(FRotator(0, 180.f, 0));

	Tags.Add(TEXT("Minion"));
}

// Called when the game starts or when spawned
void ATempMinion::BeginPlay()
{
	Super::BeginPlay();
	CurrentHP = MaxHP;
	UpdateHPBar();
}

// Called every frame
void ATempMinion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ATempMinion::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageEvent.IsOfType(FDamageEvent::ClassID))
	{
		CurrentHP -= DamageAmount;
		OnRep_CurrentHP();
	}
	
	if (CurrentHP <= 0)
	{
		ABattlePC* PC = Cast<ABattlePC>(EventInstigator);
		if (PC)
		{
			ABattleCharacter* Pawn = Cast<ABattleCharacter>(PC->GetPawn());
			if (Pawn)
			{
				Pawn->Server_SetBooty(MinionMoney, MinionExp);
				//Pawn->SetBooty(MinionMoney, MinionExp);
			}
		}
		CurrentHP = 200.0f;
		OnRep_CurrentHP();
	}
	return 0.0f;
}

void ATempMinion::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATempMinion, CurrentHP);
}

void ATempMinion::OnRep_CurrentHP()
{
	UpdateHPBar();
}

void ATempMinion::UpdateHPBar()
{
	UHPBarWidgetBase* HPWidget = Cast<UHPBarWidgetBase>(Widget->GetUserWidgetObject());
	if (HPWidget)
	{
		HPWidget->SetHPBar(CurrentHP / MaxHP);
	}
}

