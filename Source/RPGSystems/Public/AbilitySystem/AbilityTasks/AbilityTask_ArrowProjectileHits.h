// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_ArrowProjectileHits.generated.h"

class AEquipmentActor;
class AArrowActor;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UAbilityTask_ArrowProjectileHits : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "OwningAbility", HidePin = "OwningAbility", BlueprintInternalUseOnly = true))
	static UAbilityTask_ArrowProjectileHits* CreateArrowProjectile(UGameplayAbility* OwningAbility,TSubclassOf<AEquipmentActor> Class, const FTransform& SpawnTransform);
	virtual void Activate() override;
private:
	virtual void OnDestroy(bool bInOwnerFinished) override;

	void Spawn();
	
	FTransform SpawnTransform;

	TSubclassOf<AArrowActor> ArrowActorClass;
};


