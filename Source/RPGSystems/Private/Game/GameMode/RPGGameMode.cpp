// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameMode/RPGGameMode.h"

#include "Data/CharacterClassInfo.h"
#include "Data/ProjectileInfo.h"
#include "Data/RarityDataAsset.h"
#include "Data/TypeOrderDataAsset.h"

UCharacterClassInfo* ARPGGameMode::GetCharacterClassDefaultInfo() const
{
	if (!IsValid(ClassDefaults)) return nullptr;
	return ClassDefaults;
}

UProjectileInfo* ARPGGameMode::GetProjectileInfo() const
{
	if (!IsValid(ProjectileInfo)) return nullptr;
	return ProjectileInfo;
}

URarityDataAsset* ARPGGameMode::GetRarityDA() const
{
	if (!IsValid(RarityColorInfo)) return nullptr;
	return RarityColorInfo;
}

UTypeOrderDataAsset* ARPGGameMode::GetTypeOrderDA() const
{
	if (!IsValid(TypeOrderDataAsset)) return nullptr;
	return TypeOrderDataAsset;
}

