// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ScalableFloat.h"
#include "GameFramework/PlayerState.h"
#include "RPGPlayerState.generated.h"

class UGameplayEffect;
class UEquipmentManagerComponent;
class UInventoryComponent;
class URPGAttributeSet;
class URPGAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnExperienceChanged,int32 PlayerLevel, int32 NewExperience, int32 RequiredExperience)
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API ARPGPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	ARPGPlayerState();

	FOnExperienceChanged OnExperienceChangedDelegate;

	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintPure)
	URPGAbilitySystemComponent* GetRPGAbilitySystemComponent() const;

	UFUNCTION(BlueprintPure)
	URPGAttributeSet* GetRPGAttributeSet() const;
	
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_InventoryComponent)
	// TObjectPtr<UInventoryComponent> InventoryComponent;
	//
	// UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_EquipmentComponent)
	// TObjectPtr<UEquipmentManagerComponent> EquipmentComponent;

	void AddToExperience(const FScalableFloat& XPScale);
	float GetRequiredExperience() const;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnLevelUp(int32 CurrentLevel);

	UFUNCTION(BlueprintImplementableEvent)
	void OnExperienceChanged(int32 NewExperience, int32 RequiredExperience);

	int32 CurrentExperience = 0;
	int32 PlayerLevel = 1;
	
	UPROPERTY(EditDefaultsOnly,Category="Custom Values|Experience")
	FScalableFloat RequiredLevelUpExperience;
private:
	void LevelUp();

	
	// UFUNCTION()
	// void OnRep_InventoryComponent();
	//
	// UFUNCTION()
	// void OnRep_EquipmentComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<URPGAbilitySystemComponent> RPGAbilitySystemComp;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<URPGAttributeSet> RPGAttributes;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Level Up")
	TSubclassOf<UGameplayEffect> LevelUpEffect;

};



