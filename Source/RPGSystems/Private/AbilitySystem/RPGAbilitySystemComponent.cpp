// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/RPGAbilitySystemComponent.h"

#include "NativeGameplayTags.h"
#include "AbilitySystem/Abilities/ProjectileAbility.h"
#include "AbilitySystem/Abilities/RPGGameplayAbility.h"
#include "AbilitySystem/Abilities/SwordAttackAbility.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Equipment/EquipmentManagerComponent.h"

namespace RPGGameplayTags::Static
{
	UE_DEFINE_GAMEPLAY_TAG_STATIC(StatEffect_Category_Attribute, "StatEffect.Attribute");
	UE_DEFINE_GAMEPLAY_TAG_STATIC(StatEffect_Category_Ability, "StatEffect.Ability");
}


void URPGAbilitySystemComponent::OnRep_ActivateAbilities()
{
	ABILITYLIST_SCOPE_LOCK();

	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.IsActive()) continue;

		TArray<UGameplayAbility*> Instances = Spec.GetAbilityInstances();
		if (URPGGameplayAbility* RPGAbility = Cast<URPGGameplayAbility>(Instances.Last()))
		{
			if (RPGAbility->bIsClientPassive)
			{
				TryActivateAbility(Spec.Handle);
			}
		}
	}
}

void URPGAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant)
{
	for (const TSubclassOf<UGameplayAbility>& Ability : AbilitiesToGrant)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1.f);
		
		if (const URPGGameplayAbility* RPGAbility = Cast<URPGGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(RPGAbility->InputTag);
			GiveAbility(AbilitySpec);
		}
	}
}

void URPGAbilitySystemComponent::AddCharacterPassiveAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& PassivesToGrant)
{
	for (const TSubclassOf<UGameplayAbility>& Ability : PassivesToGrant)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1.f);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void URPGAbilitySystemComponent::InitializeDefaultAttributes(const TSubclassOf<UGameplayEffect>& AttributeEffect)
{
	checkf(AttributeEffect, TEXT("No valid default attributes for this character %s"), *GetAvatarActor()->GetName());
	const FGameplayEffectContextHandle ContextHandle = MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(AttributeEffect, 1.f, ContextHandle);
	ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	OnAttributesGiven.Broadcast();
}

void URPGAbilitySystemComponent::InitializeDefaultInfiniteEffects(
	const TArray<TSubclassOf<UGameplayEffect>>& StartingInfiniteEffects)
{
	if (StartingInfiniteEffects.IsEmpty()) return;
	const FGameplayEffectContextHandle ContextHandle = MakeEffectContext();
	for (const TSubclassOf<UGameplayEffect>& Effect : StartingInfiniteEffects)
	{
		const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(Effect, 1.f, ContextHandle);
		ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void URPGAbilitySystemComponent::AbilityInputPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			if (!Spec.IsActive())
			{
				TryActivateAbility(Spec.Handle);
			}
			else
			{
				UGameplayAbility* PrimaryInstance = Spec.GetPrimaryInstance();
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle,
					PrimaryInstance->GetCurrentActivationInfo().GetActivationPredictionKey());
			}
		}
	}
}

void URPGAbilitySystemComponent::AbilityInputReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			UGameplayAbility* PrimaryInstance = Spec.GetPrimaryInstance();
			if (!IsValid(PrimaryInstance))
			{
				return;
			}
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle,
					PrimaryInstance->GetCurrentActivationInfo().GetActivationPredictionKey());
		}
	}
}

void URPGAbilitySystemComponent::SetDynamicProjectile(const FGameplayTag& ProjectileTag,int32 AbilityLevel)
{
	if (!ProjectileTag.IsValid())
	{
		return;
	}
	
	if (!GetAvatarActor()->HasAuthority())
	{
		ServerSetDynamicProjectile(ProjectileTag,AbilityLevel);
		return;
	}

	if (ActiveProjectileAbility.IsValid())
	{
		ClearAbility(ActiveProjectileAbility);
	}
	
	if (IsValid(DynamicProjectileAbility))
	{
		FGameplayAbilitySpec Spec = FGameplayAbilitySpec(DynamicProjectileAbility, AbilityLevel);
		if (UProjectileAbility* ProjectileAbility = Cast<UProjectileAbility>(Spec.Ability))
		{
			ProjectileAbility->ProjectileToSpawnTag = ProjectileTag;
			Spec.GetDynamicSpecSourceTags().AddTag(ProjectileAbility->InputTag);

			ActiveProjectileAbility = GiveAbility(Spec);
		}
	}
}

void URPGAbilitySystemComponent::AddEquipmentEffects(FRPGEquipmentEntry* EquipmentEntry)
{
	FStreamableManager& Manager = UAssetManager::GetStreamableManager();
	TWeakObjectPtr<URPGAbilitySystemComponent> WeakThis(this);
	const FGameplayEffectContextHandle ContextHandle = MakeEffectContext();

	const FEquipmentStatEffectGroup& BaseDamageStat = EquipmentEntry->EffectPackage.BaseDamage;
	if (BaseDamageStat.StatEffectTag.IsValid())
	{
		AddSingleAttribute(EquipmentEntry,Manager,WeakThis,ContextHandle, BaseDamageStat);
	}
	
	const FEquipmentStatEffectGroup& ImplicitStat = EquipmentEntry->EffectPackage.Implicit;

	if (ImplicitStat.StatEffectTag.IsValid())
	{
		AddSingleEquipmentEffect(EquipmentEntry, Manager, WeakThis, ContextHandle, ImplicitStat);
	}
	
	for (const FEquipmentStatEffectGroup& StatEffect : EquipmentEntry->EffectPackage.Prefixes)
	{
		AddSingleEquipmentEffect(EquipmentEntry, Manager, WeakThis, ContextHandle, StatEffect);
	}

	for (const FEquipmentStatEffectGroup& StatEffect : EquipmentEntry->EffectPackage.Suffixes)
	{
		AddSingleEquipmentEffect(EquipmentEntry, Manager, WeakThis, ContextHandle, StatEffect);
	}
}

void URPGAbilitySystemComponent::AddSingleEquipmentEffect(FRPGEquipmentEntry* EquipmentEntry, FStreamableManager& Manager, TWeakObjectPtr<URPGAbilitySystemComponent> WeakThis, const FGameplayEffectContextHandle ContextHandle, const FEquipmentStatEffectGroup& ImplicitStat)
{
	if (ImplicitStat.StatEffectTag.MatchesTag(RPGGameplayTags::Static::StatEffect_Category_Attribute))
	{
		AddSingleAttribute(EquipmentEntry, Manager, WeakThis, ContextHandle, ImplicitStat);	
	} else if (ImplicitStat.StatEffectTag.MatchesTag(RPGGameplayTags::Static::StatEffect_Category_Ability))
	{
		for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
		{
			if (Spec.GetDynamicSpecSourceTags().HasTagExact(ImplicitStat.ContextTag))
			{
				Spec.Level = FMath::Clamp(Spec.Level + ImplicitStat.CurrentValue, 1.f, Spec.Level + ImplicitStat.CurrentValue);
			}
		}
	}
}

void URPGAbilitySystemComponent::AddSingleAttribute(FRPGEquipmentEntry* EquipmentEntry, FStreamableManager& Manager, TWeakObjectPtr<URPGAbilitySystemComponent> WeakThis, const FGameplayEffectContextHandle ContextHandle, const FEquipmentStatEffectGroup& ImplicitStat)
{
	if (IsValid(ImplicitStat.EffectClass.Get()))
	{
		const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(ImplicitStat.EffectClass.Get(),ImplicitStat.CurrentValue, ContextHandle);
		const FActiveGameplayEffectHandle ActiveHandle = ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		
		EquipmentEntry->GrantedHandles.AddEffectHandle(ActiveHandle);
	}
	else
	{
		Manager.RequestAsyncLoad(ImplicitStat.EffectClass.ToSoftObjectPath(),
								 [WeakThis,ImplicitStat,ContextHandle,EquipmentEntry]()
		 {
			 const FGameplayEffectSpecHandle SpecHandle = WeakThis->MakeOutgoingSpec(ImplicitStat.EffectClass.Get(),ImplicitStat.CurrentValue, ContextHandle);
			 const FActiveGameplayEffectHandle ActiveHandle = WeakThis->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

			 EquipmentEntry->GrantedHandles.AddEffectHandle(ActiveHandle);
		 });
	}
}

void URPGAbilitySystemComponent::RemoveEquipmentEffects(FRPGEquipmentEntry* EquipmentEntry)
{
	for (auto HandleIt = EquipmentEntry->GrantedHandles.ActiveEffects.CreateIterator(); HandleIt; ++HandleIt)
	{
		RemoveActiveGameplayEffect(*HandleIt);
		HandleIt.RemoveCurrent();
	}

	const FEquipmentStatEffectGroup& ImplicitStat = EquipmentEntry->EffectPackage.Implicit;
	if (ImplicitStat.StatEffectTag.IsValid())
	{
		if (ImplicitStat.StatEffectTag.MatchesTag(RPGGameplayTags::Static::StatEffect_Category_Ability))
		{
			for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
			{
				if (Spec.GetDynamicSpecSourceTags().HasTagExact(ImplicitStat.ContextTag))
				{
					Spec.Level = FMath::Clamp(Spec.Level - ImplicitStat.CurrentValue, 1.f, Spec.Level - ImplicitStat.CurrentValue);
				}
			}
		}
	}

	for (const FEquipmentStatEffectGroup& StatEffect : EquipmentEntry->EffectPackage.Prefixes)
	{
		if (StatEffect.StatEffectTag.MatchesTag(RPGGameplayTags::Static::StatEffect_Category_Ability))
		{
			if (!StatEffect.EffectClass.IsValid()) continue;
			for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
			{
				if (Spec.GetDynamicSpecSourceTags().HasTagExact(StatEffect.ContextTag))
				{
					Spec.Level = FMath::Clamp(Spec.Level - StatEffect.CurrentValue, 1.f, Spec.Level - StatEffect.CurrentValue);
				}
			}
		}
	}
	
	for (const FEquipmentStatEffectGroup& StatEffect : EquipmentEntry->EffectPackage.Suffixes)
	{
		if (StatEffect.StatEffectTag.MatchesTag(RPGGameplayTags::Static::StatEffect_Category_Ability))
		{
			if (!StatEffect.EffectClass.IsValid()) continue;
			for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
			{
				if (Spec.GetDynamicSpecSourceTags().HasTagExact(StatEffect.ContextTag))
				{
					Spec.Level = FMath::Clamp(Spec.Level - StatEffect.CurrentValue, 1.f, Spec.Level - StatEffect.CurrentValue);
				}
			}
		}
	}
}

void URPGAbilitySystemComponent::AddEquipmentAbility(FRPGEquipmentEntry* EquipmentEntry)
{
	FStreamableManager& Manager = UAssetManager::GetStreamableManager();
	TWeakObjectPtr<URPGAbilitySystemComponent> WeakThis(this);
	
	for (FEquipmentAbilityGroup& AbilityGroup: EquipmentEntry->EffectPackage.Abilities)
	{
		if (IsValid(AbilityGroup.AbilityClass.Get()))
		{
			EquipmentEntry->GrantedHandles.AddAbilityHandle(GrantEquipmentAbility(AbilityGroup));
			//EquipmentEntry->GrantedHandles.GrantedAbility = GrantEquipmentAbility(AbilityGroup);
			OnEquipmentAbilityGiven.Broadcast(EquipmentEntry,false);
		}
		else
		{
			Manager.RequestAsyncLoad(AbilityGroup.AbilityClass.ToSoftObjectPath(),
				[WeakThis, EquipmentEntry,AbilityGroup]()
				{
					EquipmentEntry->GrantedHandles.AddAbilityHandle(WeakThis->GrantEquipmentAbility(AbilityGroup));
					WeakThis->OnEquipmentAbilityGiven.Broadcast(EquipmentEntry,true);
				});
		}
	}
}

void URPGAbilitySystemComponent::RemoveEquipmentAbility(const FRPGEquipmentEntry* EquipmentEntry)
{
	if (!EquipmentEntry) return;

	for (const FGameplayAbilitySpecHandle& Handle : EquipmentEntry->GrantedHandles.GrantedAbilities)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (!Spec) continue;

		CancelAbilityHandle(Handle);
		ClearAbility(Handle);
	}
}

FGameplayAbilitySpecHandle URPGAbilitySystemComponent::GrantEquipmentAbility(
	const FEquipmentAbilityGroup& Ability)
{
	//FEquipmentAbilityGroup Ability = EquipmentEntry->EffectPackage.Ability;
	
	FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability.AbilityClass.Get(),Ability.AbilityLevel);

	AbilitySpec.GetDynamicSpecSourceTags().AddTag(Ability.AbilityTag);
	AbilitySpec.GetDynamicSpecSourceTags().AppendTags(Ability.AbilityContextTags);
	
	if (URPGGameplayAbility* RPGAbility = Cast<URPGGameplayAbility>(AbilitySpec.Ability))
	{
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(RPGAbility->InputTag);
	}

	if (URPGDamageAbility* DamageAbility = Cast<URPGDamageAbility>(AbilitySpec.Ability))
	{
		DamageAbility->MinDamageCoefficient = Ability.MinDamageCoefficient;
		DamageAbility->MaxDamageCoefficient = Ability.MaxDamageCoefficient;
	}

	if (UProjectileAbility* ProjectileAbility = Cast<UProjectileAbility>(AbilitySpec.Ability))
	{
		ProjectileAbility->ProjectileToSpawnTag = Ability.ContextTag;
	}

	return GiveAbility(AbilitySpec);
}


void URPGAbilitySystemComponent::ServerSetDynamicProjectile_Implementation(const FGameplayTag& ProjectileTag,int32 AbilityLevel)
{
	SetDynamicProjectile(ProjectileTag,AbilityLevel);
}
