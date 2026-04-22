#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"


namespace RPGInventoryTags
{
	namespace ItemsCategory
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Consumable);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipment);

		namespace EquipmentData
		{
			namespace Slot
			{
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(RightHand);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(LeftHand);
			}
			namespace Type
			{
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shield)
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Axe)
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Mace)
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crossbow)
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sword)
			}
		}
		namespace ConsumableType
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Food)
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Potion)
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Bomb)
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Oil)
		}
	}

	namespace Rarity
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Common);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Master);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Magic);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Relic);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Witcher);
	}
}