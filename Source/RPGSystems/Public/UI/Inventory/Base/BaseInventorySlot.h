#pragma once

#include "CoreMinimal.h"
#include "UI/Inventory/SlotSizeCategories.h"
#include "Blueprint/UserWidget.h"
#include "InventorySection/InventoryComponent.h"
#include "BaseInventorySlot.generated.h"

class UItemToolTip;
class UItemSlotIcon;
class UImage;
enum class EItemRarity : uint8;
struct FRPGInventoryEntry;
struct FMasterItemDefinition;

class UBaseInventorySlot;
DECLARE_DELEGATE_OneParam(FOnBaseSlotMouseEntered, UBaseInventorySlot*)
DECLARE_DELEGATE_OneParam(FOnBaseSlotMouseLeaved,  UBaseInventorySlot*)
DECLARE_DELEGATE_OneParam(FOnBaseSlotDoubleClicked, UBaseInventorySlot*)

UCLASS(Abstract)
class RPGSYSTEMS_API UBaseInventorySlot : public UUserWidget
{
    GENERATED_BODY()

public:
    // Tooltip linking (Superior/Lower pair)
    void SetLinkedSlot(UBaseInventorySlot* InSlot) { LinkedSlot = InSlot; }
    void CancelLeaveTimer();

    // Visual
    void SetIcon(const FSlateBrush& Brush);
    void EquipVisual(UTexture2D* Texture, EItemRarity Rarity);
    virtual void EmptySlot();

    // Animation
    virtual void StartSelectedAnimation();
    virtual void StopSelectedAnimation();

    // Getters
    bool IsEmpty()                          const { return bIsEmpty; }
    EItemRarity GetRarity()                 const { return CurrentItemDefinition.Rarity; }
    ESlotSizeCategories GetCurrentSlotSize()const { return CurrentSlotSize; }
    TSoftObjectPtr<UTexture2D> GetIconTexture() const { return CurrentItemDefinition.Icon; }
    const FRPGInventoryEntry& GetItemEntry()const { return ItemEntry; }

    // Delegates
    FOnBaseSlotMouseEntered  OnSlotMouseEnteredDelegate;
    FOnBaseSlotMouseLeaved   OnSlotMouseLeavedDelegate;
    FOnBaseSlotDoubleClicked OnSlotDoubleClickedDelegate;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    // DragDrop
    virtual void BuildDragOperation(UDragDropOperation*& OutOperation) {}

    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                      UDragDropOperation*& OutOperation) override;

    virtual void DragVisualEnable() {};
    
    // Data
    UPROPERTY(BlueprintReadOnly)
    FRPGInventoryEntry ItemEntry;
    
    FMasterItemDefinition CurrentItemDefinition;

    ESlotSizeCategories CurrentSlotSize = ESlotSizeCategories::UniqueSlot;
    bool                bIsEmpty = true;
    FSlateBrush         CurrentIconBrush;

    // Layout
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="UI")
    UImage* IconBox;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="UI")
    UImage* BackgroundRarity;

    UPROPERTY(Transient, meta=(BindWidgetAnim))
    UWidgetAnimation* SelectedSlotAnimation;

    UPROPERTY(EditDefaultsOnly, Category="Style")
    TSubclassOf<UItemSlotIcon> IconWidgetClass;

    UPROPERTY()
    UItemSlotIcon* IconWidgetReference;

    FTimerHandle ItemTooltipTimerHandle;
private:
    UPROPERTY()
    TObjectPtr<UBaseInventorySlot> LinkedSlot;

    FTimerHandle ItemTooltipLeaveTimerHandle;

    float TooltipDelay = 0.5f; // 
};