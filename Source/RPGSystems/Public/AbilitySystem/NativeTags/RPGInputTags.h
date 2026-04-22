#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace RPGInputTags
{
	namespace Inventory
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Exit);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Open);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SortItems);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SortItemsQuickly);
	}
	namespace BlockInput
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(InventoryIsOpen);
	}
}