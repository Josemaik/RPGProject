// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_KickTrace.generated.h"

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UAbilityTask_KickTrace : public UAbilityTask
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "OwningAbility", HidePin = "OwningAbility", BlueprintInternalUseOnly = true))
	static UAbilityTask_KickTrace* KickTrace(UGameplayAbility* OwningAbility, float SphereRadius = 25.f);
	virtual void Activate() override;
protected:	
	virtual void OnDestroy(bool bInOwnerFinished) override;
private:
	float SphereRadius;
};
