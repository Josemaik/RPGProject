// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_GetDistanceToTarget.generated.h"

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UBTS_GetDistanceToTarget : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTS_GetDistanceToTarget();

private:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (AllowPrivateAccess = "true"))
	FBlackboardKeySelector IsInRangeKey;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (AllowPrivateAccess = "true"))
	float MeleeAttackDistance;

	UPROPERTY()
	TObjectPtr<AActor> TargetActorRef;

	UPROPERTY()
	TObjectPtr<AAIController> AIControllerRef;

	UPROPERTY()
	TObjectPtr<UBlackboardComponent> BlackboardRef;
};
