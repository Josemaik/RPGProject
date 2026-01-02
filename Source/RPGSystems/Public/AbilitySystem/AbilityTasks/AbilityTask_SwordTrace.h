// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_SwordTrace.generated.h"

DECLARE_DELEGATE_TwoParams(FOnHitActor, AActor*,const FVector&)

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

	FOnHitActor OnHitActorDelegate;
private:
	UFUNCTION()
	void TraceCheck();

	UPROPERTY()
	TObjectPtr<UArrowComponent> TopPointArrow;
	
	UPROPERTY()
	TObjectPtr<UArrowComponent> ButtomPointArrow;
	
	FTimerHandle TraceCheckTimer;
	FTimerDelegate TraceCheckDelegate;

	TSet<TWeakObjectPtr<AActor>> HitActors;
	
	float SphereRadius;
};


