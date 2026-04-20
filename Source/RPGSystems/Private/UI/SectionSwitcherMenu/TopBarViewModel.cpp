// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SectionSwitcherMenu/TopBarViewModel.h"

//----------------------------------------------------------------------
// Computed properties
//----------------------------------------------------------------------
 
float UTopBarViewModel::GetExperiencePercent() const
{
	return RequiredExperience > 0.f ? CurrentExperience / RequiredExperience : 0.f;
}
 
bool UTopBarViewModel::GetIsOverweight() const
{
	return CurrentWeight > MaxWeight;
}
 
//----------------------------------------------------------------------
// Setters 
//----------------------------------------------------------------------
 
void UTopBarViewModel::SetPlayerLevel(int32 NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerLevel, NewValue);
}
 
void UTopBarViewModel::SetCurrentExperience(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(CurrentExperience, NewValue);
}

 
void UTopBarViewModel::SetRequiredExperience(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(RequiredExperience, NewValue);
}
 
void UTopBarViewModel::SetCurrentWeight(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(CurrentWeight, NewValue);
}
 
void UTopBarViewModel::SetMaxWeight(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(MaxWeight, NewValue);
}
 
void UTopBarViewModel::SetPreviousSectionName(FText NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(PreviousSectionName, NewValue);
}
 
void UTopBarViewModel::SetCurrentSectionName(FText NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(CurrentSectionName, NewValue);
}
 
void UTopBarViewModel::SetNextSectionName(FText NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(NextSectionName, NewValue);
}
 
