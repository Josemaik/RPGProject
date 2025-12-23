// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotifies/AnimNotify_SphereTrace.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystem/RPGGameplayTags.h"

class UAbilitySystemComponent;

void UAnimNotify_SphereTrace::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor* Owner = MeshComp->GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner);
	if (!IsValid(ASC))
	{
		return;
	}

	FGameplayEventData EventData;
	EventData.EventTag = RPGGameplayTags::Combat::Events::Actions::KickHit;

	ASC->HandleGameplayEvent(EventData.EventTag, &EventData);

	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,"KickTrace Notify");
}
