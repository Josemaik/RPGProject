// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotifies/AnimNotify_SwordComboEnd.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "AbilitySystem/RPGGameplayTags.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"

class UAbilitySystemComponent;

void UAnimNotify_SwordComboEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
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
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Black,FString::Printf(TEXT("Owner: %s"),
		*Owner->GetActorNameOrLabel()));
	
	FGameplayEventData EventData;
	EventData.EventTag = RPGGameplayTags::Combat::ComboStates::End;
	
	ASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}
