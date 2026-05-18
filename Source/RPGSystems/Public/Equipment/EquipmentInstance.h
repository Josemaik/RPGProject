// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "EquipmentInstance.generated.h"

class AEquipmentActor;
class ARPGSystemsCharacter;
struct FEquipmentActorsToSpawn;
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
	
	void SpawnEquipmentActors(const TArray<FEquipmentActorsToSpawn>& ActorsToSpawn,FGameplayTag AttachTag);
	void DestroySpawnedActors(FGameplayTag SlotTag);
	void ChangeAttachPoint(FGameplayTag OldAttachTag,FGameplayTag NewAttachTag);
	AEquipmentActor* GetActorAttached(FGameplayTag AttachPoint);
	bool HasAnActorAttached(FGameplayTag AttachPoint) const;
	
	TMap<FGameplayTag,AEquipmentActor*>& GetEquipmentActors() { return SpawnedActors; }
private:

	UPROPERTY()
	ARPGSystemsCharacter* OwnedCharacter;
	
	UPROPERTY()
	TMap<FGameplayTag,AEquipmentActor*> SpawnedActors;
	
	ACharacter* GetCharacter();
};



