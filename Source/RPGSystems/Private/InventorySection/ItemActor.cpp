// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySection/ItemActor.h"

#include "InventorySection/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Equipment/EquipmentDefinition.h"
#include "Equipment/EquipmentGenerator.h"

// Sets default values
AItemActor::AItemActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	Mesh->SetIsReplicated(true);
}

void AItemActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemActor, ItemTag);
	DOREPLIFETIME(AItemActor, NumItems);
	DOREPLIFETIME(AItemActor, ValidationBits);
	DOREPLIFETIME(AItemActor, EffectPackage);
}

void AItemActor::BeginPlay()
{
	Super::BeginPlay();

	if (bIsInialized) return;
	
	//RollForStats
	
	const UEquipmentDefinition* EquipmentCDO = GetDefault<UEquipmentDefinition>(EquipmentDefinition);
	if (!IsValid(EquipmentCDO)) return;
	ItemTag = EquipmentCDO->ItemTag;
	
	if (bRollOnBeginPlay)
	{
		UEquipmentGenerator::RollForStats(EffectPackage,EquipmentDefinition,StatEffects);
	}
	//no roll
	// if (!IsValid(ItemDefinition)) return;
	// ItemTag = ItemDefinition->ItemTag;
	// EffectPackage.Abilities = ItemDefinition->PossibleAbilityRolls;
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

	//poner boolean aqui que me indique si ha sido spawneado como loot/dropeado por player
	bIsInialized = true;
}

void AItemActor::SetMesh(UStaticMesh* InMesh)
{
	if (IsValid(InMesh))
	{
		Mesh->SetStaticMesh(InMesh);
	}
}

