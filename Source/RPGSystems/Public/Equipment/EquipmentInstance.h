// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EquipmentInstance.generated.h"

class ARPGSystemsCharacter;
struct FEquipmentActorsToSpawn;
struct FGameplayTag;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class RPGSYSTEMS_API UEquipmentInstance : public UObject
{
	GENERATED_BODY()
public:
	virtual void OnEquipped();
	virtual void OnUnEquipped();

	void SpawnEquipmentActors(const TArray<FEquipmentActorsToSpawn>& ActorsToSpawn,FGameplayTag SlotTag);
	void DestroySpawnedActors(FGameplayTag SlotTag);

private:

	UPROPERTY()
	ARPGSystemsCharacter* OwnedCharacter;
	
	UPROPERTY()
	TArray<AActor*> SpawnedActors;
	
	ACharacter* GetCharacter();
};


