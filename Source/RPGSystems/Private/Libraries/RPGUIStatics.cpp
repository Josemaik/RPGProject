// Fill out your copyright notice in the Description page of Project Settings.


#include "Libraries/RPGUIStatics.h"

#include "Data/RarityDataAsset.h"
#include "Data/TypeOrderDataAsset.h"
#include "Game/GameMode/RPGGameMode.h"
#include "Kismet/GameplayStatics.h"


FLinearColor URPGUIStatics::GetColorByRarity(const UObject* WorldContext, FGameplayTag RarityTag)
{
	if (!RarityTag.IsValid()) return FLinearColor::Gray;
	
	ARPGGameMode* RPGGameMode = Cast<ARPGGameMode>(UGameplayStatics::GetGameMode(WorldContext));
	if (!IsValid(RPGGameMode)) return FLinearColor::White;

	URarityDataAsset* RarityColorDA = RPGGameMode->GetRarityDA();
	if (!IsValid(RarityColorDA)) return FLinearColor::White;
	
	return RarityColorDA->RarityColors.Find(RarityTag)->Color;
}

FText URPGUIStatics::GetTextForRarity(const UObject* WorldContext, FGameplayTag Raritytag)
{
	ARPGGameMode* RPGGameMode = Cast<ARPGGameMode>(UGameplayStatics::GetGameMode(WorldContext));
	if (!IsValid(RPGGameMode)) return FText();

	URarityDataAsset* RarityColorDA = RPGGameMode->GetRarityDA();
	if (!IsValid(RarityColorDA)) return FText();
	
	return RarityColorDA->RarityColors.Find(Raritytag)->RarityText;
}

int32 URPGUIStatics::GetRarityTagPriority(const UObject* WorldContext, FGameplayTag Raritytag)
{
	ARPGGameMode* RPGGameMode = Cast<ARPGGameMode>(UGameplayStatics::GetGameMode(WorldContext));
	if (!IsValid(RPGGameMode)) return INT32_MAX;

	URarityDataAsset* RarityDA = RPGGameMode->GetRarityDA();
	if (!IsValid(RarityDA)) return INT32_MAX;

	int32 Index = RarityDA->RarityTagsPriority.IndexOfByKey(Raritytag);
	return Index == INDEX_NONE ? INT32_MAX : Index;
}

int32 URPGUIStatics::GetTypePriorityInCategory(const UObject* WorldContex,const FGameplayTag& Category,const FGameplayTag& TypeTag)
{
	ARPGGameMode* RPGGameMode = Cast<ARPGGameMode>(UGameplayStatics::GetGameMode(WorldContex));
	if (!IsValid(RPGGameMode)) return INT32_MAX;

	UTypeOrderDataAsset* TypeDA = RPGGameMode->GetTypeOrderDA();
	if (!IsValid(TypeDA)) return INT32_MAX;

	const FGameplayTagArray* OrderArray = TypeDA->TypeOrderByCategory.Find(Category);
	if (!OrderArray) return INT32_MAX;

	const int32 Index = OrderArray->Tags.IndexOfByKey(TypeTag);
	return Index != INDEX_NONE ? Index : INT32_MAX;
}





