// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MotionWarpingComponent.h"
#include "RPGMotionWarpingComponent.generated.h"

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API URPGMotionWarpingComponent : public UMotionWarpingComponent
{
	GENERATED_BODY()
public:
	void SetLockedTarget(AActor* Target);
	void UpdateAttackWarpTarget();
private:
	void FindAndSetNearestEnemyTarget();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> LockedTarget;
};
