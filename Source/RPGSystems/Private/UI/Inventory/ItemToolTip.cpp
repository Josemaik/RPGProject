// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemToolTip.h"

#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "InventorySection/ItemTypes.h"
#include "Libraries/RPGUIStatics.h"

void UItemToolTip::SetData(const FMasterItemDefinition& ItemDefinition)
{
	ItemNameText->SetText(ItemDefinition.ItemName);
	if (ItemDefinition.Rarity == EItemRarity::Common)
	{
		ItemTypeText->SetText(FText::FromString("OTHER"));
	}
	ItemDescriptionText->SetText(ItemDefinition.ItemDescription);
	FLinearColor RarityColor = URPGUIStatics::GetColorForRarity(GetWorld(),ItemDefinition.Rarity);
	RarityBorder->SetBrushColor(FLinearColor(RarityColor.R, RarityColor.G, RarityColor.B, 0.6));

	//Add prefixes,sufixes,Implicits
	//for (ItemDefinition.ConsumableProps.)
	
	RarityText->SetText(URPGUIStatics::GetTextForRarity(GetWorld(),ItemDefinition.Rarity));
	LevelText->SetText(FText::FromString("1")); //to-do
	WeightText->SetText(FText::AsNumber(ItemDefinition.Weight));
	DurabilityText->SetText(FText::AsNumber(77)); // to-do
	PriceText->SetText(FText::AsNumber(ItemDefinition.Price));
}
