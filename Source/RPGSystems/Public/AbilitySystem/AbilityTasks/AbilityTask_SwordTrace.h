// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_SwordTrace.generated.h"

class UArrowComponent;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UAbilityTask_SwordTrace : public UAbilityTask
{
	GENERATED_BODY()
public:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
	
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "OwningAbility", HidePin = "OwningAbility", BlueprintInternalUseOnly = true))
	static UAbilityTask_SwordTrace* ShordTrace(UGameplayAbility* OwningAbility, float SphereRadius = 12.f);
private:
	UFUNCTION()
	void TraceCheck();

	UPROPERTY()
	TObjectPtr<UArrowComponent> TopPointArrow;
	
	UPROPERTY()
	TObjectPtr<UArrowComponent> ButtomPointArrow;
	
	FTimerHandle TraceCheckTimer;
	FTimerDelegate TraceCheckDelegate;

	float SphereRadius;
};


