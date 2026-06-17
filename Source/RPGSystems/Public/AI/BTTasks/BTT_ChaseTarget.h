// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_ChaseTarget.generated.h"

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UBTT_ChaseTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTT_ChaseTarget();
private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (AllowPrivateAccess = "true"))
	FBlackboardKeySelector TargetActorBlackboardKey;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = "true"))
	float AcceptanceRadius { 100.f };
};
