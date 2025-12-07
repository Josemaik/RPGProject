// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ScalableFloat.h"
#include "Character/CharacterBase.h"
#include "Interfaces/RPGAbilitySystemInterface.h"
#include "EnemyBase.generated.h"

class ULootSpawnInfo;
class UInventoryComponent;
class URPGAttributeSet;
class URPGAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API AEnemyBase : public ACharacterBase, public IAbilitySystemInterface, public IRPGAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AEnemyBase(const FObjectInitializer& ObjectInitializer);

	/*Implement RPG Ability System Interface*/
	virtual void AddAttackingActor_Implementation(AActor* AttackingActor) override;
	virtual void Death_Implementation() override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:

	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitClassDefaults() override;
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;
private:

	void SpawnLoot();
	
	UPROPERTY(ReplicatedUsing=OnRep_InitAttributes)
	bool bInitAttributes = false;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<URPGAbilitySystemComponent> RPGAbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<URPGAttributeSet> RPGAttributeSet;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInventoryComponent> InventoryComp;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Loot")
	TObjectPtr<ULootSpawnInfo> LootSpawnInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Loot")
	FGameplayTagContainer LootCategoryTags;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Loot")
	FGameplayTagContainer SpecificLootTags;

	FScalableFloat AwardedExperienceScale;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> AttackingActors;

	UFUNCTION()
	void OnRep_InitAttributes();
};


