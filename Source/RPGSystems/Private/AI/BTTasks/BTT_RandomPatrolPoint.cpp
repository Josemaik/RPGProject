// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTT_RandomPatrolPoint.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_RandomPatrolPoint::UBTT_RandomPatrolPoint()
{
	NodeName = TEXT("Find Random Location");

	BlackboardKey.AddVectorFilter(this,GET_MEMBER_NAME_CHECKED(
		UBTT_RandomPatrolPoint, BlackboardKey));
}

EBTNodeResult::Type UBTT_RandomPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!IsValid(AIController)) return EBTNodeResult::Failed;

	const APawn* Pawn = AIController->GetPawn();

	//Get pawn origin
	const FVector Origin = Pawn->GetActorLocation();

	FNavLocation NewLocation = FNavLocation(Origin);
	
	const UNavigationSystemV1* NavSystem{UNavigationSystemV1::GetCurrent(GetWorld())};
	if (!IsValid(NavSystem) || !NavSystem->GetRandomPointInNavigableRadius(Origin,SearchRadius, NewLocation)) return EBTNodeResult::Failed;
	
	AIController->GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName, NewLocation);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

FString UBTT_RandomPatrolPoint::GetStaticDescription() const
{
	return FString::Printf(TEXT("Vector: %s"), *BlackboardKey.SelectedKeyName.ToString());
}
