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
}

namespace RPGGameplayTags::Player::State
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Crouch, "Player.State.Crouch", "Player is Actively Crouching");
}

