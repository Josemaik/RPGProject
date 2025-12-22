// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotifies/AnimNotify_SwordComboContinue.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "AbilitySystem/RPGGameplayTags.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"

void UAnimNotify_SwordComboContinue::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor* Owner = MeshComp->GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	ACharacter* Avatar = Cast<ACharacter>(Owner);
	if (!IsValid(Avatar))
	{
		return;
	}
	
	APlayerState* PlayerState = Avatar->GetPlayerState();
	if (!IsValid(PlayerState))
	{
		return;
	}
	UAbilitySystemComponent* ASC = PlayerState->FindComponentByClass<URPGAbilitySystemComponent>();
	if (!IsValid(ASC))
	{
		return;
	}

	FGameplayEventData EventData;
	EventData.EventTag = RPGGameplayTags::Combat::ComboStates::Start;

	ASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}
