// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AssasinationAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Components/RPGMotionWarpingComponent.h"
#include "Interfaces/AssasinationInterface.h"

UAssasinationAbility::UAssasinationAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UAssasinationAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,"Assasination Ability");
}

void UAssasinationAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	TArray<AActor*> OverlappingActors;

	if (!ActorInfo || !AssasinationAnimMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (!AvatarActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	//Motion Warping Comp
	MotionWarpingCompRef = AvatarActor->FindComponentByClass<URPGMotionWarpingComponent>();

	if (!MotionWarpingCompRef.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	FVector TargetLocation;
	FRotator TargetRotation;
	bool bFoundTarget = false;
	
	AvatarActor->GetOverlappingActors(OverlappingActors);
	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor->Implements<UAssasinationInterface>())
		{
			if (IAssasinationInterface* AssasinationInterface = Cast<IAssasinationInterface>(OverlappingActor))
			{
				AssasinationInterface->StealthBackAssasin(TargetLocation,TargetRotation);
				ActorAssasinated = OverlappingActor;
				bFoundTarget = true;
				break;
			}
		}
	}

	if (!bFoundTarget)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	//Play Montage
	PlayAssasinationAnimation(TargetLocation,TargetRotation);
}

void UAssasinationAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAssasinationAbility::PlayAssasinationAnimation(FVector& TargetLocation,FRotator& TargetRotation)
{
	//Motion Warping target
	FMotionWarpingTarget WarpTarget;
	WarpTarget.Name = "AssasinationWarp";
	WarpTarget.Location = TargetLocation;
	WarpTarget.Rotation = TargetRotation;
	MotionWarpingCompRef->AddOrUpdateWarpTarget(WarpTarget);
	
	UAbilityTask_PlayMontageAndWait* MontageTask =
	UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		AssasinationAnimMontage
	);
	
	MontageTask->OnCompleted.AddDynamic(this, &UAssasinationAbility::OnAssasinationMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UAssasinationAbility::OnAssasinationMontageFailed);
	MontageTask->OnCancelled.AddDynamic(this, &UAssasinationAbility::OnAssasinationMontageFailed);

	MontageTask->ReadyForActivation();
}

void UAssasinationAbility::OnAssasinationMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	ActorAssasinated->Destroy();
}

void UAssasinationAbility::OnAssasinationMontageFailed()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}


