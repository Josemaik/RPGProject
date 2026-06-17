// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTT_ChaseTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "Navigation/PathFollowingComponent.h"

UBTT_ChaseTarget::UBTT_ChaseTarget()
{
	NodeName = TEXT("Chase Target");
}

EBTNodeResult::Type UBTT_ChaseTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!IsValid(AIController)) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject(TargetActorBlackboardKey.SelectedKeyName));
	if (!IsValid(TargetActor)) return EBTNodeResult::Failed;
	
	EPathFollowingRequestResult::Type Result = AIController->MoveToActor(TargetActor);
	if (Result == EPathFollowingRequestResult::Type::RequestSuccessful)
	{
		return EBTNodeResult::Succeeded;
	}
	if (Result == EPathFollowingRequestResult::Type::Failed)
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Succeeded;
}

FString UBTT_ChaseTarget::GetStaticDescription() const
{
		return FString::Printf(TEXT("Target: %s \n Stop Distance: %f"), *TargetActorBlackboardKey.SelectedKeyName.ToString(),AcceptanceRadius);
}
