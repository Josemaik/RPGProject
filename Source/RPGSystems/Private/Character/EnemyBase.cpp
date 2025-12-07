// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/EnemyBase.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/RPGAttributeSet.h"
#include "Data/CharacterClassInfo.h"
#include "Libraries/RPGAbilitySystemLibrary.h"
#include "Net/UnrealNetwork.h"

AEnemyBase::AEnemyBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bReplicates = true;

	RPGAbilitySystemComponent = CreateDefaultSubobject<URPGAbilitySystemComponent>("AbilitySystemComponent");
	RPGAbilitySystemComponent->SetIsReplicated(true);
	RPGAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	RPGAttributeSet = CreateDefaultSubobject<URPGAttributeSet>("AttributeSet");
}

void AEnemyBase::AddAttackingActor_Implementation(AActor* AttackingActor)
{
	if (!AttackingActors.Contains(AttackingActor))
	{
		AttackingActors.Emplace(AttackingActor);
	}
}

void AEnemyBase::Death_Implementation()
{
	if (!HasAuthority()) return;

	if (!AttackingActors.IsEmpty())
	{
		for (AActor* Attacker : AttackingActors)
		{
			if (IsValid(Attacker))
			{
				Execute_AddToExperience(Attacker, AwardedExperienceScale);
			}
		}
	}
	Destroy();
}

UAbilitySystemComponent* AEnemyBase::GetAbilitySystemComponent() const
{
	return RPGAbilitySystemComponent;
}

void AEnemyBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyBase, bInitAttributes);
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	BindCallbacksToDependencies();
	InitAbilityActorInfo();
}

void AEnemyBase::InitAbilityActorInfo()
{
	if (!IsValid(RPGAbilitySystemComponent) || !IsValid(RPGAttributeSet))
	{
		return;
	}

	RPGAbilitySystemComponent->InitAbilityActorInfo(this, this);

	if (HasAuthority())
	{
		InitClassDefaults();
		BroadcastInitialValues();
	}
}

void AEnemyBase::InitClassDefaults()
{
	if (!CharacterTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("No character tag selected in this character: %s"), *GetNameSafe(this));
		return;
	}

	if (!IsValid(RPGAbilitySystemComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability System Component invalid: %s"), *GetNameSafe(this));
		return;
	}
	
	if (UCharacterClassInfo* ClassInfo = URPGAbilitySystemLibrary::GetCharacterClassDefaultInfo(this))
	{
		if (const FCharacterClassDefaultInfo* SelectedClass = ClassInfo->ClassDefaultInfoMap.Find(CharacterTag))
		{
			RPGAbilitySystemComponent->AddCharacterAbilities(SelectedClass->StartingAbilities);
			RPGAbilitySystemComponent->AddCharacterAbilities(SelectedClass->StartingPassives);
			RPGAbilitySystemComponent->InitializeDefaultAttributes(SelectedClass->DefaultAttributes);
			
			AwardedExperienceScale = SelectedClass->AwardedExperienceScale;
		}
	}
}

void AEnemyBase::BindCallbacksToDependencies()
{
	if (!IsValid(RPGAbilitySystemComponent) || !IsValid(RPGAttributeSet))
	{
		return;
	}

	RPGAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(RPGAttributeSet->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged(Data.NewValue, RPGAttributeSet->GetMaxHealth());
		});

	RPGAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(RPGAttributeSet->GetShieldAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnShieldChanged(Data.NewValue, RPGAttributeSet->GetMaxShield());
		});

	if (HasAuthority())
	{
		RPGAbilitySystemComponent->OnAttributesGiven.AddLambda(
			[this]
			{
				bInitAttributes = true;
			});
	}
}

void AEnemyBase::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();

	if (!IsValid(RPGAttributeSet))
	{
		return;
	}

	OnHealthChanged(RPGAttributeSet->GetHealth(), RPGAttributeSet->GetMaxHealth());
	OnShieldChanged(RPGAttributeSet->GetShield(), RPGAttributeSet->GetMaxShield());
}

void AEnemyBase::OnRep_InitAttributes()
{
	BroadcastInitialValues();
}
