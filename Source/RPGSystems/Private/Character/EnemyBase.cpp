// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/EnemyBase.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/RPGAttributeSet.h"
#include "AbilitySystem/NativeTags/RPGGameplayTags.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Data/CharacterClassInfo.h"
#include "Data/LootSpawnInfo.h"
#include "InventorySection/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
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

	InventoryComp = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");

	OverHeadBar = CreateDefaultSubobject<UWidgetComponent>("OverHeadWidget");
	OverHeadBar->SetupAttachment(GetCapsuleComponent());
	OverHeadBar->SetWidgetSpace(EWidgetSpace::Screen);
	OverHeadBar->SetDrawAtDesiredSize(true);
	OverHeadBar->SetRelativeLocation(FVector(0.f, 0.f, 110.f));
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

	GetWorld()->GetTimerManager().SetTimer(
		RagDollTimerHandle1,
		this,
		&AEnemyBase::EnterRagdoll,
		0.5f,
		false
	);
}
void AEnemyBase::EnterRagdoll()
{
	GetMesh()->SetSimulatePhysics(true);
	
	GetWorld()->GetTimerManager().SetTimer(
		SpawnLootTimerHandle,
		this,
		&AEnemyBase::OnRagdollFinished,
		2.5f,
		false
	);
}

void AEnemyBase::OnRagdollFinished()
{
	//SpawnLoot();
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

void AEnemyBase::SetOverHeadBar()
{
	if (!IsValid(OverHeadBar) || !IsValid(OverHeadWidgetClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("OverHeadBar component is null"));
		return;
	}
	
	OverHeadBar->SetWidgetClass(OverHeadWidgetClass);
	OverHeadBar->InitWidget();

	UUserWidget* Widget = OverHeadBar->GetUserWidgetObject();
	if (!IsValid(Widget))
	{
		UE_LOG(LogTemp, Warning, TEXT("OverHeadBar widget not created yet"));
		return;
	}

	OverHeadBarWidget = Widget;
	InitializeHealthBar();
}

void AEnemyBase::RegisterCombatEvents()
{
	if (RPGAbilitySystemComponent)
	{
		RPGAbilitySystemComponent
			->RegisterGameplayTagEvent(
				RPGGameplayTags::Combat::Events::State::HitReact, 
				EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &AEnemyBase::OnHitReactTagChanged);
	}
	
	USkeletalMeshComponent* MeshComponent = GetMesh();
	if (!IsValid(MeshComponent)) return;
	AnimInstanceReference = MeshComponent->GetAnimInstance();
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	BindCallbacksToDependencies();
	InitAbilityActorInfo();
	RegisterCombatEvents();
	SetOverHeadBar();


	PlayerRef = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
}

void AEnemyBase::OnHitReactTagChanged(
	const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		if (HitReactMontages.IsEmpty())
		{
			return;
		}

		const int32 RandomIndex = FMath::RandRange(0, HitReactMontages.Num() - 1);
		UAnimMontage* HitReactMontage = HitReactMontages[RandomIndex];

		if (!IsValid(HitReactMontage) || !IsValid(AnimInstanceReference))
		{
			return;
		}

		AnimInstanceReference->Montage_Play(HitReactMontage);

		if (!IsValid(PlayerRef)) return;
		FVector LaunchVelocity = PlayerRef->GetActorForwardVector() * LAUNCHIMPULSEONHIT;
		
		LaunchCharacter(LaunchVelocity,false,false);
	}
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
	RPGAbilitySystemComponent->AddLooseGameplayTag(CharacterTag);
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

void AEnemyBase::SpawnLoot()
{
	check(InventoryComp);
	check(LootSpawnInfo);

	FGameplayTagContainer AllLootTags;
	AllLootTags.AppendTags(URPGAbilitySystemLibrary::GetAllChildrenTagsOfCategories(LootCategoryTags));
	AllLootTags.AppendTags(SpecificLootTags);

	bool bShouldSpawn = true;
	while (bShouldSpawn)
	{
		const FGameplayTag& RandomTag = URPGAbilitySystemLibrary::GetRandomTagFromContainer(AllLootTags);
		
		for (const auto& Pair : LootSpawnInfo->TaggedLootTables)
		{
			if (RandomTag.MatchesTag(Pair.Key))
			{
				if (const FPossibleLootParams* PossibleLoot = URPGAbilitySystemLibrary::GetDataTableRowByTag<FPossibleLootParams>(Pair.Value, RandomTag))
				{
					if (FMath::FRandRange(0.f, 1.f) < PossibleLoot->ProbabilityToSelect)
					{
						const int32 RandomNumItems = FMath::RandRange(PossibleLoot->MinNumItems, PossibleLoot->MaxNumItems);
						if (const FRPGInventoryEntry* Result = InventoryComp->InventoryList.AddItem(PossibleLoot->ResultingItemtag, RandomNumItems))
						{
							InventoryComp->SpawnItem(GetActorTransform(), Result, Result->Quantity);
							bShouldSpawn = false;
							break;
						}
						break;
					}
					break;
				}
				break;
			}
		}
	}
}

void AEnemyBase::InitializeHealthBar_Implementation()
{
}

void AEnemyBase::OnRep_InitAttributes()
{
	BroadcastInitialValues();
}
