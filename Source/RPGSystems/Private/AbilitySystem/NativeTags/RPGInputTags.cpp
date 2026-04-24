#include "AbilitySystem/NativeTags/RPGInputTags.h"

namespace RPGInputTags
{
	namespace Inventory
	{
		UE_DEFINE_GAMEPLAY_TAG(Exit,"Input.Inventory.Exit");
		UE_DEFINE_GAMEPLAY_TAG(Open,"Input.Inventory.Open");
		UE_DEFINE_GAMEPLAY_TAG(SortItems,"Input.Inventory.SortItems");
		UE_DEFINE_GAMEPLAY_TAG(SortItemsQuickly,"Input.Inventory.SortItemsQuickly");
		UE_DEFINE_GAMEPLAY_TAG(DropItem,"Input.Inventory.DropItem");
		UE_DEFINE_GAMEPLAY_TAG(EquipItem,"Input.Inventory.EquipItem");
	}

	namespace BlockInput
	{
		UE_DEFINE_GAMEPLAY_TAG(InventoryIsOpen,"Input.BlockInput.InventoryOpen");
	}
}