#include "AbilitySystem/NativeTags/RPGInventoryTags.h"

namespace RPGInventoryTags
{
    // Category
    namespace ItemsCategory
    {
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment,  "Item.Equipment",  "Key TagsToTables → DT_Equipment");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Consumable, "Item.Consumable", "Key TagsToTables → DT_Consumables");
    }

    // SubCategory
    namespace ItemsSubCategory
    {
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapons,  "Item.Equipment.Weapons",   "Subcategoría equipment");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Armor,    "Item.Equipment.Armor",     "Subcategoría equipment");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Utility,  "Item.Equipment.Utility",   "Subcategoría equipment");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Potions,  "Item.Consumable.Potions",  "Subcategoría consumable");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Bombs,    "Item.Consumable.Bombs",    "Subcategoría consumable");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Oils,     "Item.Consumable.Oils",     "Subcategoría consumable");
    }

    // Type
    namespace ItemType
    {
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sword,     "Item.Type.Sword",    "");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Axe,       "Item.Type.Axe",      "");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Crossbow,  "Item.Type.Crossbow", "");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Torch,     "Item.Type.Torch",    "");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Shield,    "Item.Type.Shield",   "");
    }

    // Rarity
    namespace Rarity
    {
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Common,   "Item.Rarity.Common",   "");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Magic,    "Item.Rarity.Magic",    "");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Master,   "Item.Rarity.Master",   "");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Relic,    "Item.Rarity.Relic",    "");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Witcher,  "Item.Rarity.Witcher",  "");
    }

    // Equipment Slot UI
    namespace EquipmentSlot
    {
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(SilverWeapon, "Equipment.Slot.SilverWeapon", "Witcher: espada de plata");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(SteelWeapon,  "Equipment.Slot.SteelWeapon",  "Witcher: espada de acero");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Bolts,  "Equipment.Slot.Bolts",  "Witcher: bolts");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Chest,        "Equipment.Slot.Chest",        "");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gauntlets,    "Equipment.Slot.Gauntlets",    "");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Trousers,     "Equipment.Slot.Trousers",     "");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Boots,        "Equipment.Slot.Boots",        "");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Consumable,   "Equipment.Slot.Consumable",   "");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Bombs,        "Equipment.Slot.Bombs",        "");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Utility,      "Equipment.Slot.Utility",      "");
    }

    // AttachPoint
    namespace AttachPoint
    {
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(RightHand, "Equipment.AttachPoint.RightHand", "hand_r");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(LeftHand,  "Equipment.AttachPoint.LeftHand",  "hand_l");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Back,      "Equipment.AttachPoint.Back",      "spine_03");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Back_Sword,"Equipment.AttachPoint.Back_Sword_Slot","Socket silver sword");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Hip,       "Equipment.AttachPoint.Hip",       "");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Chest,     "Equipment.AttachPoint.Chest",     "");
    }
}