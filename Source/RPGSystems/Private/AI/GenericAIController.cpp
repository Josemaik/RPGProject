// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GenericAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"


// Sets default values
AGenericAIController::AGenericAIController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	
	UAISenseConfig_Sight* SightSenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightSenseConfig"));
	SightSenseConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightSenseConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightSenseConfig->DetectionByAffiliation.bDetectFriendlies = true;
	
	AIPerceptionComponent->ConfigureSense(*SightSenseConfig);
	AIPerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());
	
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AGenericAIController::OnTargetPerceptionUpdated);
}

// Called when the game starts or when spawned
void AGenericAIController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(GenericBehaviorTree))
	{
		return;
	}
	
	RunBehaviorTree(GenericBehaviorTree);
}

void AGenericAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	UE_LOG(LogTemp, Warning, TEXT("OnTargetPerceptionUpdated llamado, Actor: %s"), Actor ? *Actor->GetName() : TEXT("null"));
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			GetBlackboardComponent()->SetValueAsObject(FName(TEXT("TargetActor")),Actor);
		}
		else
		{
			GetBlackboardComponent()->SetValueAsObject(FName(TEXT("TargetActor")),nullptr);
		}
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		// hear
		GetBlackboardComponent()->SetValueAsObject(FName(TEXT("TargetActor")),nullptr);
	}
}

inline void AGenericAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	AIPerceptionComponent->OnTargetPerceptionUpdated.RemoveAll(this);
}



