// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentGenerator.h"

#include "Data/EquipmentStaffEfects.h"
#include "Equipment/EquipmentDefinition.h"
#include "Libraries/RPGAbilitySystemLibrary.h"

void UEquipmentGenerator::RollForStats(FEquipmentEffectPackage& OutPackage,const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition,TObjectPtr<UEquipmentStaffEfects> WeakStats)
{
	if (!IsValid(EquipmentDefinition) || !IsValid(WeakStats.Get()))
	{
		return;
	}
	
	UEquipmentStaffEfects* StatEffects = WeakStats.Get();
	const UEquipmentDefinition* EquipmentCDO = GetDefault<UEquipmentDefinition>(EquipmentDefinition);

	if (EquipmentCDO->BaseDamage.DamageTypeTag.IsValid())
	{
		OutPackage.BaseDamage.StatEffectTag = EquipmentCDO->BaseDamage.DamageTypeTag;
		OutPackage.BaseDamage.EffectClass = EquipmentCDO->BaseDamage.EffectClass;
		OutPackage.BaseDamage.CurrentValue = EquipmentCDO->BaseDamage.EffectLevel;
	}
	
	if (EquipmentCDO->bForceRollAllAbilities)
	{
		for (FGameplayTag GameplayTag : EquipmentCDO->PossibleAbilityRolls)
		{
			for (const auto& Pair :StatEffects->MasterStatMap)
			{
				if (const FEquipmentAbilityGroup* PossibleAbility = URPGAbilitySystemLibrary::GetDataTableRowByTag<FEquipmentAbilityGroup>(Pair.Value, GameplayTag))
				{
					OutPackage.Abilities.Add(*PossibleAbility);
				}
			}
		}
	}
	else
	{
		if (EquipmentCDO->PossibleAbilityRolls.IsEmpty())
		{
			return;
		}
		
		bool bShouldRoll = true;
		int32 NumRollAbilities = 0;
		while (bShouldRoll)
		{
			const FGameplayTag& RandomTag = URPGAbilitySystemLibrary::GetRandomTagFromContainer(EquipmentCDO->PossibleAbilityRolls);

			for (const auto& Pair :StatEffects->MasterStatMap)
			{
				if (RandomTag.MatchesTag(Pair.Key))
				{
					if (const FEquipmentAbilityGroup* PossibleAbility = URPGAbilitySystemLibrary::GetDataTableRowByTag<FEquipmentAbilityGroup>(Pair.Value, RandomTag))
					{
						if (FMath::RandRange(0.f, 1.f) <= PossibleAbility->ProabilityToSelect)
						{
							OutPackage.Abilities.Add(*PossibleAbility);
							NumRollAbilities++;
							if (NumRollAbilities >= EquipmentCDO->MinNumRollAbilities)
							{
								bShouldRoll = false;
							}
							break;
						}
					}
				}
			}
		}
	}
		

	FGameplayTag ImplicitTag = EquipmentCDO->PossibleStatsRolls.ImplicitTag;
	if (ImplicitTag.IsValid())
	{
		for (const auto& Pair: StatEffects->MasterStatMap)
		{
			if (ImplicitTag.MatchesTag(Pair.Key))
			{
				if (const FEquipmentStatEffectGroup* ValidStat = URPGAbilitySystemLibrary::GetDataTableRowByTag<FEquipmentStatEffectGroup>(Pair.Value,
					EquipmentCDO->PossibleStatsRolls.ImplicitTag))
				{
					FEquipmentStatEffectGroup NewStat = *ValidStat;

					NewStat.CurrentValue = ValidStat->bFractionalStat ? FMath::FRandRange(ValidStat->MinStatLevel, ValidStat->MaxStatLevel) :
						FMath::TruncToInt(FMath::FRandRange(ValidStat->MinStatLevel, ValidStat->MaxStatLevel));

					OutPackage.Implicit = NewStat;
				}
			}
		}
	}

	FGameplayTagContainer AllPossiblePrefixes;
	AllPossiblePrefixes.AppendTags(URPGAbilitySystemLibrary::GetAllChildrenTagsOfCategories(EquipmentCDO->PossibleStatsRolls.PrefixCategoryTags));
	AllPossiblePrefixes.AppendTags(EquipmentCDO->PossibleStatsRolls.SpecificPrefixTags);
	AllPossiblePrefixes.RemoveTags(EquipmentCDO->PossibleStatsRolls.PrefixExclusionTags);
	
	FGameplayTagContainer AllPossibleSuffixes;
	AllPossibleSuffixes.AppendTags(URPGAbilitySystemLibrary::GetAllChildrenTagsOfCategories(EquipmentCDO->PossibleStatsRolls.SuffixCategoryTags));
	AllPossibleSuffixes.AppendTags(EquipmentCDO->PossibleStatsRolls.SpecificSuffixTags);
	AllPossibleSuffixes.RemoveTags(EquipmentCDO->PossibleStatsRolls.SuffixExclusionTags);
	
	const int32 NumStatsToRoll = FMath::RandRange(EquipmentCDO->MinPossibleStats, EquipmentCDO->MaxPossibleStats);
	int32 StatRollIndex = 0;
	while (StatRollIndex < NumStatsToRoll)
	{
		if (FMath::RandBool() && OutPackage.Prefixes.Num() < OutPackage.MaxNumPrefixes)
		{
			RollPrefixOrSuffix(Prefix, StatEffects, OutPackage, AllPossiblePrefixes);
		} else if (OutPackage.Suffixes.Num() < OutPackage.MaxNumSuffixes)
		{
			RollPrefixOrSuffix(Suffix, StatEffects, OutPackage, AllPossibleSuffixes);
		}
		else if (OutPackage.Prefixes.Num() < OutPackage.MaxNumPrefixes)
		{
			RollPrefixOrSuffix(Prefix, StatEffects, OutPackage, AllPossiblePrefixes);
		}
		++StatRollIndex;
	}	
}

void UEquipmentGenerator::RollPrefixOrSuffix(EEquipmentStatsGroup StatGroup, UEquipmentStaffEfects* StatEffects,
	FEquipmentEffectPackage& OutPackage, FGameplayTagContainer& PossibleStatContainer)
{
	const FGameplayTag& RandomTag = URPGAbilitySystemLibrary::GetRandomTagFromContainer(PossibleStatContainer);

	for (const auto& Pair : StatEffects->MasterStatMap)
	{
		if (RandomTag.MatchesTag(Pair.Key))
		{
			if (const FEquipmentStatEffectGroup* PossibleStat = URPGAbilitySystemLibrary::GetDataTableRowByTag<FEquipmentStatEffectGroup>(Pair.Value, RandomTag))
			{
				if (FMath::FRandRange(0.f, 1.f) < PossibleStat->ProbabilityToSelect)
				{
					FEquipmentStatEffectGroup NewStat = *PossibleStat;

					NewStat.CurrentValue = PossibleStat->bFractionalStat ? FMath::FRandRange(PossibleStat->MinStatLevel, PossibleStat->MaxStatLevel) :
					FMath::TruncToInt(FMath::FRandRange(PossibleStat->MinStatLevel, PossibleStat->MaxStatLevel));

					switch (StatGroup)
					{
					case Prefix:
						{
							OutPackage.Prefixes.Add(NewStat);
							break;
						}
					case Suffix:
						{
							OutPackage.Suffixes.Add(NewStat);
							break;
						}
					default:{}
					}
					
					
					// make unique stats
					PossibleStatContainer.RemoveTag(RandomTag); 
					FName RandomTagName = RandomTag.GetTagLeafName(); //Stats.LowLevel.Health => Health
					FGameplayTagContainer TagsToRemove;
					
					for (const FGameplayTag& Tag : PossibleStatContainer)
					{
						if (Tag.GetTagLeafName() == RandomTagName)
						{
							TagsToRemove.AddTag(Tag);
						}
					}
					
					PossibleStatContainer.RemoveTags(TagsToRemove);
					
					break;
				}
			}
		}
	}
}
