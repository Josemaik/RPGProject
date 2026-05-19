// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AbilityTask_ArrowProjectileHits.h"

#include "Equipment/EquipmentActors/EquipmentActor.h"

void UAbilityTask_ArrowProjectileHits::Activate()
{
	Super::Activate();

	
}

void UAbilityTask_ArrowProjectileHits::Spawn()
{
	
}

void UAbilityTask_ArrowProjectileHits::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}

UAbilityTask_ArrowProjectileHits* UAbilityTask_ArrowProjectileHits::CreateArrowProjectile(UGameplayAbility* OwningAbility,TSubclassOf<AEquipmentActor> Class,
                                                                               const FTransform& SpawnTransform)
{
	UAbilityTask_ArrowProjectileHits* MyObj = NewAbilityTask<UAbilityTask_ArrowProjectileHits>(OwningAbility);

	MyObj->Ability = OwningAbility;
	MyObj->SpawnTransform = SpawnTransform;
	MyObj->ArrowActorClass = Class;
	
	return MyObj;
}
