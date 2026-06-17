// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTT_AttackTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interfaces/EnemyAIInterface.h"

UBTT_AttackTarget::UBTT_AttackTarget()
{
	NodeName = TEXT("Attack Target");
}

void UBTT_AttackTarget::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
	
}

EBTNodeResult::Type UBTT_AttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!IsValid(AIController)) return EBTNodeResult::Failed;
	
	APawn* ActorPawn = AIController->GetPawn();
	if (!IsValid(ActorPawn)) return EBTNodeResult::Failed;

	if (ActorPawn->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass()))
	{
		IEnemyAIInterface* EnemyAIInterface = Cast<IEnemyAIInterface>(ActorPawn);
		if (EnemyAIInterface == nullptr) return EBTNodeResult::Failed;
		
		float AnimationAttackTime = EnemyAIInterface->Attack();

		OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName(TEXT("InMeleeAttackRange")),false);

		TWeakObjectPtr<UBehaviorTreeComponent> WeakOwnerComp(&OwnerComp);
		OwnerComp.GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda(
			[this, WeakOwnerComp]()
			{
				FinishLatentTask(*WeakOwnerComp.Get(),EBTNodeResult::Succeeded);
			}),
			AnimationAttackTime,
			false
			);
		
		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

FString UBTT_AttackTarget::GetStaticDescription() const
{
	return Super::GetStaticDescription();
}
