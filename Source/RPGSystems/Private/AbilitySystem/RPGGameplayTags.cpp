// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/RPGGameplayTags.h"

namespace RPGGameplayTags::Combat
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data_Damage, "Combat.Data.Damage", "Set by Caller Data Tag For Combat");
	//UE_DEFINE_GAMEPLAY_TAG_STATIC(Projectile_Basic, "Projectile.Basic");
	namespace DamageTypes
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(DamageTypeParent, "Combat.DamageType", "Primary Parent For Damage Types")
	}
	namespace Events
	{
		namespace Melee
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(StartCombo, "Combat.Event.SwordComboStart","Combo has started");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(EndCombo, "Combat.Event.SwordComboEnd", "Combo has ended");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitStart, "Combat.Event.HitStart", "Start Check hits");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitEnd, "Combat.Event.HitEnd", "End Check hits");
		}

		namespace Actions
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(KickHit, "Combat.Event.Kick","kick has been thrown");
		}
	}
}

namespace RPGGameplayTags::Player::State
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Crouch, "Player.State.Crouch", "Player is Actively Crouching");
}

namespace RPGGameplayTags::InventoryItems
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ConsumableTag,"Item.Consumable","Inventory Item consumable by player");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(EquipmentTag,"Item.Equipment","Inventory Item by player");
}

