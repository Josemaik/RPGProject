// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTServices/BTS_GetDistanceToTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_GetDistanceToTarget::UBTS_GetDistanceToTarget()
{
	NodeName = TEXT("Service Get Distance To Target");
	bNotifyTick = true;
}

void UBTS_GetDistanceToTarget::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	// AIControllerRef = OwnerComp.GetAIOwner();
	// if (!IsValid(AIControllerRef)) return;
	//
	// BlackboardRef = AIControllerRef->GetBlackboardComponent();
	// if (!IsValid(BlackboardRef)) return;
	//
	// TargetActorRef = Cast<AActor>(BlackboardRef->GetValueAsObject(BlackboardKey.SelectedKeyName));
}

void UBTS_GetDistanceToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!IsValid(AIController))
		return;

	UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
	if (!IsValid(Blackboard))
		return;

	AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(BlackboardKey.SelectedKeyName));
	if (!IsValid(Target))
		return;

	float Distance = AIController->GetPawn()->GetDistanceTo(Target);
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Distance: %f"),Distance));

	Blackboard->SetValueAsBool(
		IsInRangeKey.SelectedKeyName,
		Distance <= MeleeAttackDistance
	);
}

FString UBTS_GetDistanceToTarget::GetStaticDescription() const
{
	return FString::Printf(TEXT("Target: %s \n MeleeDist: %f \n IsInRange: %s"), *BlackboardKey.SelectedKeyName.ToString(),MeleeAttackDistance,*IsInRangeKey.SelectedKeyName.ToString());
}
