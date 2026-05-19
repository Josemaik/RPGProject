// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentActors/CrossBowActor.h"

#include "Components/ArrowComponent.h"


// Sets default values
ACrossBowActor::ACrossBowActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(GetRootComponent());
	
	MuzzleDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("MuzzleDirection"));
	MuzzleDirection->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ACrossBowActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACrossBowActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

