// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "TopBarViewModel.generated.h"

/**
 * ViewModel for TopBarWidget.
 * Holds all display data for the top bar: experience, weight, and section navigation.
 * SectionSwitcherWidget writes to this; TopBarWidget binds to it via the MVVM editor panel.
 */
UCLASS()
class RPGSYSTEMS_API UTopBarViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

	//----------------------------------------------------------------------
	// Experience
	//----------------------------------------------------------------------
	UPROPERTY(FieldNotify, BlueprintReadWrite, Getter, Setter, meta = (AllowPrivateAccess))
	int32 PlayerLevel = 1;
 
	UPROPERTY(FieldNotify, BlueprintReadWrite, Getter, Setter, meta = (AllowPrivateAccess))
	float CurrentExperience = 0.f;
 
	UPROPERTY(FieldNotify, BlueprintReadWrite, Getter, Setter, meta = (AllowPrivateAccess))
	float RequiredExperience = 100.f;
 
	//----------------------------------------------------------------------
	// Weight
	//----------------------------------------------------------------------
	UPROPERTY(FieldNotify, BlueprintReadWrite, Getter, Setter, meta = (AllowPrivateAccess))
	float CurrentWeight = 0.f;
 
	UPROPERTY(FieldNotify, BlueprintReadWrite, Getter, Setter, meta = (AllowPrivateAccess))
	float MaxWeight = 100.f;
 
	//----------------------------------------------------------------------
	// Section Navigation
	//----------------------------------------------------------------------
	UPROPERTY(FieldNotify, BlueprintReadWrite, Getter, Setter, meta = (AllowPrivateAccess))
	FText PreviousSectionName;
 
	UPROPERTY(FieldNotify, BlueprintReadWrite, Getter, Setter, meta = (AllowPrivateAccess))
	FText CurrentSectionName;
 
	UPROPERTY(FieldNotify, BlueprintReadWrite, Getter, Setter, meta = (AllowPrivateAccess))
	FText NextSectionName;
 
public:
	//----------------------------------------------------------------------
	// Computed (FieldNotify functions) — updated automatically by MVVM
	// when their source properties change. Bind these directly in editor.
	//----------------------------------------------------------------------
 
	/** 0.0 – 1.0  →  bind to ExperienceBar.Percent */
	UFUNCTION(FieldNotify, BlueprintPure)
	float GetExperiencePercent() const;
 
	/** true when CurrentWeight > MaxWeight  →  drive color on CurrentInventoryWeightText */
	UFUNCTION(FieldNotify, BlueprintPure)
	bool GetIsOverweight() const;
 
	//----------------------------------------------------------------------
	// Setters  (called by SectionSwitcherWidget, never by the widget itself)
	//----------------------------------------------------------------------
	void SetPlayerLevel(int32 NewValue);
	void SetCurrentExperience(float NewValue);
	void SetRequiredExperience(float NewValue);
	void SetCurrentWeight(float NewValue);
	void SetMaxWeight(float NewValue);
	void SetPreviousSectionName(FText NewValue);
	void SetCurrentSectionName(FText NewValue);
	void SetNextSectionName(FText NewValue);
 
	//----------------------------------------------------------------------
	// Getters  (required by the Getter specifier on each UPROPERTY)
	//----------------------------------------------------------------------
	int32   GetPlayerLevel()          const { return PlayerLevel; }
	float   GetCurrentExperience()    const { return CurrentExperience; }
	float   GetRequiredExperience()   const { return RequiredExperience; }
	float   GetCurrentWeight()        const { return CurrentWeight; }
	float   GetMaxWeight()            const { return MaxWeight; }
	FText   GetPreviousSectionName()  const { return PreviousSectionName; }
	FText   GetCurrentSectionName()   const { return CurrentSectionName; }
	FText   GetNextSectionName()      const { return NextSectionName; }
};
