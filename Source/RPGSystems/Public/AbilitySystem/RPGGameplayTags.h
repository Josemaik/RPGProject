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

	namespace ComboStates
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Start);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(End);
	}
}

namespace RPGGameplayTags::Player::State
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crouch);
}
