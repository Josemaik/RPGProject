#pragma once


UENUM(BlueprintType)
enum class EUISections : uint8
{
	NONE  UMETA(DisplayName = "None"),
	CHARACTERBUILD  UMETA(DisplayName = "Character Build"),
	INVENTORY       UMETA(DisplayName = "Inventory"),
	WORLDMAP        UMETA(DisplayName = "World Map"),
	MISSIONS        UMETA(DisplayName = "Missions")
};
