#pragma once

#include "ItemSortTypes.generated.h"

UENUM(BlueprintType)
enum class EItemSortType : uint8
{
	Type        UMETA(DisplayName = "Type"),
	Price       UMETA(DisplayName = "Price"),
	Weight      UMETA(DisplayName = "Weight"),
	Durability  UMETA(DisplayName = "Durability"),
	Rarity      UMETA(DisplayName = "Rarity"),
	MAX         UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EItemSortType, EItemSortType::MAX)


