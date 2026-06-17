// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_RandomPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UBTT_RandomPatrolPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTT_RandomPatrolPoint();
private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = "true"))
	float SearchRadius { 500.f };
};
