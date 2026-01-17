// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace RPGGameplayTags::Combat
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Damage);

	namespace DamageTypes
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(DamageTypeParent);
	}

	namespace Events
	{
		namespace Melee
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(StartCombo);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(EndCombo);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitStart);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitEnd);
		}

		namespace Actions
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(KickHit);
		}
	}
}

namespace RPGGameplayTags::Player::State
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crouch);
}

namespace RPGGameplayTags::InventoryItems
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ConsumableTag);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentTag);
}
