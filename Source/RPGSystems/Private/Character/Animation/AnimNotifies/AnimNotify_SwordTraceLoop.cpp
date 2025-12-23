// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotifies/AnimNotify_SwordTraceLoop.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystem/RPGGameplayTags.h"

class UAbilitySystemComponent;

void UAnimNotify_SwordTraceLoop::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             float TotalDuration)
{
	TriggerGameplayEvent(MeshComp,RPGGameplayTags::Combat::Events::Melee::HitStart);
}

void UAnimNotify_SwordTraceLoop::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	TriggerGameplayEvent(MeshComp,RPGGameplayTags::Combat::Events::Melee::HitEnd);
}

void UAnimNotify_SwordTraceLoop::TriggerGameplayEvent(USkeletalMeshComponent* MeshComp, FGameplayTag EventTag)
{
	AActor* Owner = MeshComp->GetOwner();
	if (!IsValid(Owner)) return;

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner);
	if (!IsValid(ASC)) return;
	
	FGameplayEventData EventData;
	EventData.EventTag = EventTag;

	ASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}
