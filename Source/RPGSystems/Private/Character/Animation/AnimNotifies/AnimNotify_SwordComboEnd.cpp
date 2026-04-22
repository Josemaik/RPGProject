// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotifies/AnimNotify_SwordComboEnd.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystem/NativeTags/RPGGameplayTags.h"

class UAbilitySystemComponent;

void UAnimNotify_SwordComboEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
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
	EventData.EventTag = RPGGameplayTags::Combat::Events::Melee::EndCombo;
	
	ASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}
