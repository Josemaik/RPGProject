// Fill out your copyright notice in the Description page of Project Settings.


#include "Libraries/RPGAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystem/RPGAbilityTypes.h"
#include "AbilitySystem/RPGGameplayTags.h"
#include "Game/GameMode/RPGGameMode.h"
#include "Kismet/GameplayStatics.h"

UCharacterClassInfo* URPGAbilitySystemLibrary::GetCharacterClassDefaultInfo(const UObject* WorldContextObject)
{
	const ARPGGameMode* const RPGGameMode = Cast<ARPGGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (!IsValid(RPGGameMode))
	{
		return nullptr;
	}
	
	return RPGGameMode->GetCharacterClassDefaultInfo();
}

UProjectileInfo* URPGAbilitySystemLibrary::GetProjectileInfo(const UObject* WorldContextObject)
{
	const ARPGGameMode* const RPGGameMode = Cast<ARPGGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (!IsValid(RPGGameMode))
	{
		return nullptr;
	}
	
	return RPGGameMode->GetProjectileInfo();
}

void URPGAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectInfo& DamageEffectInfo)
{
	FGameplayEffectContextHandle ContextHandle = DamageEffectInfo.SourceASC->MakeEffectContext();
	ContextHandle.AddSourceObject(DamageEffectInfo.AvatarActor);

	const FGameplayEffectSpecHandle SpecHandle = DamageEffectInfo.SourceASC->MakeOutgoingSpec(DamageEffectInfo.DamageEffect, DamageEffectInfo.AbilityLevel, ContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, RPGGameplayTags::Combat::Data_Damage,DamageEffectInfo.BaseDamage);
	if (IsValid(DamageEffectInfo.TargetASC))
	{
		DamageEffectInfo.TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}
