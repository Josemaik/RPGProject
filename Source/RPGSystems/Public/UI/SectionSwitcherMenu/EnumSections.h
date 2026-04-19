#pragma once


UENUM(BlueprintType)
enum class EUISections : uint8
{
	CHARACTERBUILD  UMETA(DisplayName = "Character Build"),
	INVENTORY       UMETA(DisplayName = "Inventary"),
	WORLDMAP        UMETA(DisplayName = "World Map"),
	MISSIONS        UMETA(DisplayName = "Missions")
};
