// Fill out your copyright notice in the Description page of Project Settings.



#include "Game/PlayerState/RPGPlayerState.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/RPGAttributeSet.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "InventorySection/InventoryComponent.h"
#include "Net/UnrealNetwork.h"

ARPGPlayerState::ARPGPlayerState()
{
	SetReplicates(true);
	
	SetNetUpdateFrequency(100.f);
	SetMinNetUpdateFrequency(66.f);

	RPGAbilitySystemComp = CreateDefaultSubobject<URPGAbilitySystemComponent>("AbilitySystemComp");
	RPGAbilitySystemComp->SetIsReplicated(true);
	RPGAbilitySystemComp->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	RPGAttributes = CreateDefaultSubobject<URPGAttributeSet>("AttributeSet");

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetIsReplicated(true);
	
	EquipmentComponent = CreateDefaultSubobject<UEquipmentManagerComponent>(TEXT("EquipmentManagerComponent"));
	EquipmentComponent->SetIsReplicated(true);

}

void ARPGPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGPlayerState, InventoryComponent);
	DOREPLIFETIME(ARPGPlayerState, EquipmentComponent);
}

void ARPGPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (InventoryComponent)
		InventoryComponent->InventoryList.OwningObject = this;

	if (EquipmentComponent)
		EquipmentComponent->EquipmentList.OwningObject = this;
}

UAbilitySystemComponent* ARPGPlayerState::GetAbilitySystemComponent() const
{
	return RPGAbilitySystemComp;
}

URPGAbilitySystemComponent* ARPGPlayerState::GetRPGAbilitySystemComponent() const
{
	return RPGAbilitySystemComp;
}

URPGAttributeSet* ARPGPlayerState::GetRPGAttributeSet() const
{
	return RPGAttributes;
}

void ARPGPlayerState::LevelUp()
{
	check(LevelUpEffect);

	if (IsValid(RPGAbilitySystemComp))
	{
		FGameplayEffectContextHandle ContextHandle = RPGAbilitySystemComp->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = RPGAbilitySystemComp->MakeOutgoingSpec(LevelUpEffect, PlayerLevel, ContextHandle);
		RPGAbilitySystemComp->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}

	OnLevelUp(PlayerLevel);
}

void ARPGPlayerState::AddToExperience(const FScalableFloat& XPScale)
{
	CurrentExperience += XPScale.GetValueAtLevel(PlayerLevel);
	const float RequiredExperience = RequiredLevelUpExperience.GetValueAtLevel(PlayerLevel);

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue,
	FString::Printf(TEXT("%s Gained %.2f Experience"),*GetOwner()->GetName(),XPScale.GetValueAtLevel(PlayerLevel)));
	
	if (CurrentExperience >= RequiredExperience)
	{
		PlayerLevel++;
		CurrentExperience = CurrentExperience - RequiredExperience;
		LevelUp();
	}

	OnExperienceChangedDelegate.Broadcast(PlayerLevel,CurrentExperience,RequiredExperience);
	OnExperienceChanged(CurrentExperience,RequiredExperience);
}

float ARPGPlayerState::GetRequiredExperience() const
{
	return RequiredLevelUpExperience.GetValueAtLevel(PlayerLevel);
}

void ARPGPlayerState::OnRep_InventoryComponent()
{
	InventoryComponent->InventoryList.OwningObject = this;
}

void ARPGPlayerState::OnRep_EquipmentComponent()
{
	EquipmentComponent->EquipmentList.OwningObject = this;
}

