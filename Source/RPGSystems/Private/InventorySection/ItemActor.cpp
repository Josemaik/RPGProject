// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySection/ItemActor.h"

#include "InventorySection/InventoryComponent.h"


// Sets default values
AItemActor::AItemActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	Mesh->SetIsReplicated(true);
}

void AItemActor::SetParams(const FRPGInventoryEntry* Entry, int32 InNumItems)
{
	if (!Entry)
	{
		return;
	}

	ItemTag = Entry->ItemTag;
	EffectPackage = Entry->EffectPackage;
	NumItems = InNumItems;
}

void AItemActor::SetMesh(UStaticMesh* InMesh)
{
	if (IsValid(InMesh))
	{
		Mesh->SetStaticMesh(InMesh);
	}
}

