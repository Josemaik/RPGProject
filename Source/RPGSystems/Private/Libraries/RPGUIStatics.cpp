// Fill out your copyright notice in the Description page of Project Settings.


#include "Libraries/RPGUIStatics.h"

#include "Data/RarityDataAsset.h"
#include "Game/GameMode/RPGGameMode.h"
#include "Kismet/GameplayStatics.h"


FLinearColor URPGUIStatics::GetColorForRarity(const UObject* WorldContext, EItemRarity Rarity)
{
	ARPGGameMode* RPGGameMode = Cast<ARPGGameMode>(UGameplayStatics::GetGameMode(WorldContext));
	if (!IsValid(RPGGameMode)) return FLinearColor::White;

	URarityDataAsset* RarityColorDA = RPGGameMode->GetRPGUIStatics();
	if (!IsValid(RarityColorDA)) return FLinearColor::White;
	
	return RarityColorDA->RarityColors.Find(Rarity)->Color;
}

FText URPGUIStatics::GetTextForRarity(const UObject* WorldContext, EItemRarity Rarity)
{
	ARPGGameMode* RPGGameMode = Cast<ARPGGameMode>(UGameplayStatics::GetGameMode(WorldContext));
	if (!IsValid(RPGGameMode)) return FText();

	URarityDataAsset* RarityColorDA = RPGGameMode->GetRPGUIStatics();
	if (!IsValid(RarityColorDA)) return FText();
	
	return RarityColorDA->RarityColors.Find(Rarity)->RarityText;
}



