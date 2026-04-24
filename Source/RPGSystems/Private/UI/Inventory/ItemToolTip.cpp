// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemToolTip.h"

#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "InventorySection/ItemTypes.h"
#include "Libraries/RPGUIStatics.h"

void UItemToolTip::SetData(const FMasterItemDefinition& ItemDefinition)
{
	ItemNameText->SetText(ItemDefinition.ItemName);
	ItemDescriptionText->SetText(ItemDefinition.ItemDescription);
	FLinearColor RarityColor = URPGUIStatics::GetColorForRarity(GetWorld(),ItemDefinition.Rarity);
	RarityBorder->SetBrushColor(RarityColor);

	//Add prefixes,sufixes,Implicits
	
	RarityText->SetText(URPGUIStatics::GetTextForRarity(GetWorld(),ItemDefinition.Rarity));
	LevelText->SetText(FText::FromString("1")); //to-do
	WeightText->SetText(FText::AsNumber(ItemDefinition.Weight));
	DurabilityText->SetText(FText::AsNumber(77)); // to-do
	PriceText->SetText(FText::AsNumber(ItemDefinition.Price));
}
