// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AbilityTask_InteractTrace.h"

#include "AbilitySystemComponent.h"
#include "Interfaces/InteractableInterface.h"

UAbilityTask_InteractTrace::UAbilityTask_InteractTrace()
{
	bTickingTask = true;
}

UAbilityTask_InteractTrace* UAbilityTask_InteractTrace::InteractTrace(UGameplayAbility* OwningAbility,float InMaxInteractRange)
{
	//return NewAbilityTask<UAbilityTask_InteractTrace>(OwningAbility);
	UAbilityTask_InteractTrace* MyObj = NewAbilityTask<UAbilityTask_InteractTrace>(OwningAbility);
	
	MyObj->Ability = OwningAbility;
	MyObj->MaxInteractDistance = InMaxInteractRange;
	MyObj->AvatarActor = MyObj->GetAvatarActor();
	
	return MyObj;
}

void UAbilityTask_InteractTrace::Activate()
{
	Super::Activate();

	if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{
		PlayerController = ASC->AbilityActorInfo->PlayerController.Get();
	}
}

void UAbilityTask_InteractTrace::TickTask(float DeltaTime)
{
	if (!IsValid(PlayerController) || !IsValid(AvatarActor))
	{
		EndTask();
	}
	Super::TickTask(DeltaTime);
	PlayerController->GetHitResultUnderCursor(ECC_Visibility,false,InteractTraceHit);

	AActor* HitActor = InteractTraceHit.GetActor();
	ThisFrameInteractActor = HitActor;
	
	if (ThisFrameInteractActor)
	{
		const FVector InteractableLocation = HitActor->GetActorLocation();
		const FVector PlayerLocation = AvatarActor->GetActorLocation();
		if (FVector::Dist(InteractableLocation, PlayerLocation) >= MaxInteractDistance)
		{
			ThisFrameInteractActor = nullptr;
		}
		else if (ThisFrameInteractActor != LastFrameInteractActor)
		{
			//ThisFrameInteractActor->BeginInteraact(PlayerController);
			IInteractableInterface::Execute_BeginInteract(ThisFrameInteractActor.GetObject(),PlayerController);
		}
	}
	if (LastFrameInteractActor)
	{
		if (LastFrameInteractActor != ThisFrameInteractActor)
		{
			IInteractableInterface::Execute_EndInteract(LastFrameInteractActor.GetObject());
		}
	}
	
	LastFrameInteractActor = ThisFrameInteractActor;
}
