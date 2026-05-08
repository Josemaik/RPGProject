// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemToolTip.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "InventorySection/ItemTypes.h"
#include "Libraries/RPGUIStatics.h"

void UItemToolTip::SetData(const FMasterItemDefinition& ItemDefinition)
{
	ItemNameText->SetText(ItemDefinition.ItemName);
	// if (ItemDefinition.RarityTag == /*gameplay tag common*/)
	// {
	// 	ItemTypeText->SetText(FText::FromString("OTHER"));
	// }

	FString TagString = ItemDefinition.SlotTag.GetTagName().ToString();
	FString LeafString;
	TagString.Split(TEXT("."), nullptr, &LeafString, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

	ItemTypeText->SetText(FText::FromString(LeafString));
	
	ItemDescriptionText->SetText(ItemDefinition.ItemDescription);
	FLinearColor RarityColor = URPGUIStatics::GetColorByRarity(GetWorld(),ItemDefinition.RarityTag);
	RarityBorder->SetBrushColor(FLinearColor(RarityColor.R, RarityColor.G, RarityColor.B, 0.6));

	//Add prefixes,sufixes,Implicits
	//for (ItemDefinition.ConsumableProps.)
	
	RarityText->SetText(URPGUIStatics::GetTextForRarity(GetWorld(),ItemDefinition.RarityTag));
	LevelText->SetText(FText::FromString("1")); //to-do
	WeightText->SetText(FText::AsNumber(ItemDefinition.Weight));
	DurabilityText->SetText(FText::AsNumber(77)); // to-do
	PriceText->SetText(FText::AsNumber(ItemDefinition.Price));
}
