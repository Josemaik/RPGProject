// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentActor.h"


AEquipmentActor::AEquipmentActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootScene);
	
	EquipmentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	EquipmentMesh->SetupAttachment(RootComponent);
	EquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


