#include "AbilitySystem/NativeTags/RPGInventoryTags.h"

namespace RPGInventoryTags
{
	namespace ItemsCategory
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Consumable,"Item.Consumable","Inventory Item consumable by player");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment,"Item.Equipment","Inventory Item by player");

		namespace EquipmentData
		{
			namespace Slot
			{
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(RightHand,"Equipment.Slot.RightHand","Equipment equipped in player right hand");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(LeftHand,"Equipment.Slot.LeftHand","Equipment equipped in player left hand");
			}

			namespace Type
			{
				UE_DEFINE_GAMEPLAY_TAG(Shield,"Item.Equipment.LeftHand.Shield");
				UE_DEFINE_GAMEPLAY_TAG(Axe,"Item.Equipment.RightHand.Axe");
				UE_DEFINE_GAMEPLAY_TAG(Mace,"Item.Equipment.RightHand.Mace");
				UE_DEFINE_GAMEPLAY_TAG(Crossbow,"Item.Equipment.RightHand.Crossbow");
				UE_DEFINE_GAMEPLAY_TAG(Sword,"Item.Equipment.RightHand.Sword");
			}
		}
		namespace ConsumableType
		{
			UE_DEFINE_GAMEPLAY_TAG(Potion,"Item.Consumable.Potion");
			UE_DEFINE_GAMEPLAY_TAG(Bomb,"Item.Consumable.Bomb");
		}
	}

	namespace Rarity
	{
		UE_DEFINE_GAMEPLAY_TAG(Common,"Item.Rarity.Common");
		UE_DEFINE_GAMEPLAY_TAG(Master,"Item.Rarity.Master");
		UE_DEFINE_GAMEPLAY_TAG(Magic,"Item.Rarity.Magic");
		UE_DEFINE_GAMEPLAY_TAG(Relic,"Item.Rarity.Relic");
		UE_DEFINE_GAMEPLAY_TAG(Witcher,"Item.Rarity.Witcher");
	}
}
