// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ScalableFloat.h"
#include "Character/CharacterBase.h"
#include "Interfaces/RPGAbilitySystemInterface.h"
#include "EnemyBase.generated.h"

class UWidgetComponent;
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

	UFUNCTION(BlueprintNativeEvent)
	void InitializeHealthBar();
protected:

	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitClassDefaults() override;
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;
private:
	void SetOverHeadBar();
	void RegisterCombatEvents();
	void SpawnLoot();
	
	UFUNCTION()
	void OnRep_InitAttributes();

	UFUNCTION()
	void OnHitReactTagChanged(const FGameplayTag Tag, int32 NewCount);

	UFUNCTION()
	void EnterRagdoll();

	UFUNCTION()
	void OnRagdollFinished();
	
	UPROPERTY(ReplicatedUsing=OnRep_InitAttributes)
	bool bInitAttributes = false;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<URPGAbilitySystemComponent> RPGAbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<URPGAttributeSet> RPGAttributeSet;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInventoryComponent> InventoryComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> OverHeadBar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI",meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> OverHeadWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Loot")
	TObjectPtr<ULootSpawnInfo> LootSpawnInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Loot")
	FGameplayTagContainer LootCategoryTags;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Loot")
	FGameplayTagContainer SpecificLootTags;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Animation")
	TArray<TObjectPtr<UAnimMontage>> HitReactMontages;
	
	FScalableFloat AwardedExperienceScale;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> AttackingActors;

	UPROPERTY(BlueprintReadOnly,meta=(allowPrivateAccess=true))
	TObjectPtr<UUserWidget> OverHeadBarWidget;

	UPROPERTY()
	TObjectPtr<UAnimInstance> AnimInstanceReference;

	UPROPERTY()
	TObjectPtr<ACharacter> PlayerRef;

	FTimerHandle SpawnLootTimerHandle;
	FTimerHandle RagDollTimerHandle1;

	const float LAUNCHIMPULSEONHIT = 700.f;
};


