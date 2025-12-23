// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotifies/AnimNotify_SwordComboContinue.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystem/RPGGameplayTags.h"

void UAnimNotify_SwordComboContinue::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
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
	EventData.EventTag = RPGGameplayTags::Combat::Events::Melee::StartCombo;

	ASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}
