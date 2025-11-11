// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemRowWidget.h"

#include "GameplayTagContainer.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "InventorySection/InventoryComponent.h"

void UItemRowWidget::SetActionText(const FGameplayTag& GameplayTag)
{
	if (GameplayTag.MatchesTag(GameplayTagConsumable))
	{
		ActionText->SetText(FText::FromString("Use"));
		return;
	}
	
	if (GameplayTag.MatchesTag(GameplayTagEquipment))
	{
		ActionText->SetText(FText::FromString("Equip"));
		return;
	}
	
	ActionText->SetText(FText::FromString("Not a equipment"));
	ActionButtom->SetIsEnabled(false); //Disable buttom
}

void UItemRowWidget::SetItemNameText(FText Text)
{
	ItemName->SetText(Text);
}

void UItemRowWidget::SetQuantityText(int32 Quantity)
{
	const FText FormatText = FText::FromString(FString("x") + FString::FromInt(Quantity));
	ItemQuantity->SetText(FormatText);
}

void UItemRowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ActionButtom->OnClicked.AddDynamic(this,&UItemRowWidget::OnClickedUseButtom);
}

void UItemRowWidget::SetInventoryEntry(const FRPGInventoryEntry& Entry)
{
	ItemEntry = Entry;
}

FReply UItemRowWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		OnItemRowClickedDelegate.ExecuteIfBound(ItemEntry);
	}
	return FReply::Handled();
}

void UItemRowWidget::OnClickedUseButtom()
{
	OnUseButtomClickedDelegate.ExecuteIfBound(ItemEntry);
}
