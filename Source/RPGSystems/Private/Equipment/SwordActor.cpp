// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentActors/SwordActor.h"
#include "Components/ArrowComponent.h"


// Sets default values
ASwordActor::ASwordActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SwordTopPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("SwordTopPoint"));
	SwordTopPoint->SetupAttachment(EquipmentMesh);

	SwordButtomPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("SwordButtomPoint"));
	SwordButtomPoint->SetupAttachment(EquipmentMesh);
}



