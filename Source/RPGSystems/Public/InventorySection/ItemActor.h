// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Equipment/EquipmentTypes.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "ItemActor.generated.h"

class UEquipmentStaffEfects;
class UEquipmentDefinition;
class UItemTypesToTables;
struct FRPGInventoryEntry;

UCLASS()
class RPGSYSTEMS_API AItemActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
public:
	AItemActor();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	
	void SetParams(const FRPGInventoryEntry* Entry, int32 InNumItems);
	void SetMesh(UStaticMesh* InMesh);
	// begin play
	// si soy instancia del mundo -> asignar datos con item definition y rollforstats
	// Billboard o seleccionar mesh para preview en el mundo
	
	UPROPERTY(Replicated, EditAnywhere,BlueprintReadOnly,Category="Custom Values|Data")
	FGameplayTag ItemTag;
	
	UPROPERTY(Replicated, EditAnywhere,BlueprintReadOnly,Category="Custom Values|Data")
	int32 NumItems = 1;

	UPROPERTY(Replicated)
	uint8 ValidationBits = 0;

	UPROPERTY(Replicated, EditAnywhere,BlueprintReadOnly,Category="Custom Values|Data")
	FEquipmentEffectPackage EffectPackage;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Definitions")
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Stat Effect")
	TObjectPtr<UEquipmentStaffEfects> StatEffects;
	
private:

	UPROPERTY(VisibleAnywhere, meta=(allowPrivateAccess=true))
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly,Category="Custom Values|Definitions")
	bool bRollOnBeginPlay = false;
	
	bool bIsInialized = false;
};
