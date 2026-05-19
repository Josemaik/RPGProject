#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"


#pragma once

#include "CoreMinimal.h"

namespace RPGInventoryTags
{
    // Category
    namespace ItemsCategory
    {
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipment);   // Item.Equipment
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Consumable);  // Item.Consumable
    }

    // Subcategory
    namespace ItemsSubCategory
    {
        // Equipment
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapons);     // Item.Equipment.Weapons
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Armor);       // Item.Equipment.Armor
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Utility);     // Item.Equipment.Utility
        // Consumable
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Potions);     // Item.Consumable.Potions
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Bombs);       // Item.Consumable.Bombs
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Oils);        // Item.Consumable.Oils
    }

    // Type
    namespace ItemType
    {
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sword);       // Item.Type.Sword
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Axe);         // Item.Type.Axe
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crossbow);    // Item.Type.Crossbow
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Torch);       // Item.Type.Torch
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shield);      // Item.Type.Shield
    }

    // Rarity
    namespace Rarity
    {
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Common);      // Item.Rarity.Common
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Magic);       // Item.Rarity.Magic
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Master);      // Item.Rarity.Master
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Relic);       // Item.Rarity.Relic
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Witcher);     // Item.Rarity.Witcher
    }

    // Equipment Slot
    namespace EquipmentSlot
    {
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(SilverWeapon); // Equipment.Slot.SilverWeapon
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(SteelWeapon);  // Equipment.Slot.SteelWeapon
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Bolts); //Equipment.Slot.Bolts
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Chest);         // Equipment.Slot.Chest
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gauntlets);     // Equipment.Slot.Gauntlets
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trousers);      // Equipment.Slot.Trousers
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Boots);         // Equipment.Slot.Boots
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Consumable);    // Equipment.Slot.Consumable
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Bombs);         // Equipment.Slot.Bombs
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Utility);       // Equipment.Slot.Utility
    }

    // Attach Point
    namespace AttachPoint
    {
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(RightHand);   // Equipment.AttachPoint.RightHand
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(LeftHand);    // Equipment.AttachPoint.LeftHand
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Back);        // Equipment.AttachPoint.Back
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hip);         // Equipment.AttachPoint.Hip
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Chest);       // Equipment.AttachPoint.Chest
    }
}