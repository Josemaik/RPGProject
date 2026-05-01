// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentInstance.h"


#include "AbilitySystem/NativeTags/RPGInventoryTags.h"
#include "Character/RPGSystemsCharacter.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Equipment/EquipmentActor.h"
#include "Equipment/EquipmentDefinition.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"

void UEquipmentInstance::OnEquipped()
{
}

void UEquipmentInstance::OnUnEquipped()
{
}

void UEquipmentInstance::SpawnEquipmentActors(const TArray<FEquipmentActorsToSpawn>& ActorsToSpawn,FGameplayTag SlotTag)
{
	OwnedCharacter = Cast<ARPGSystemsCharacter>(GetCharacter());
	if (IsValid(OwnedCharacter))
	{
		FStreamableManager& Manager = UAssetManager::GetStreamableManager();
		TWeakObjectPtr<UEquipmentInstance> WeakThis(this);
		
		for (const FEquipmentActorsToSpawn& ActorToSpawn : ActorsToSpawn)
		{
			if (IsValid(ActorToSpawn.EquipmentClass.Get()))
			{
				AEquipmentActor* NewActor = GetWorld()->SpawnActorDeferred<AEquipmentActor>(ActorToSpawn.EquipmentClass.Get(),
					FTransform::Identity, OwnedCharacter);
				NewActor->FinishSpawning(FTransform::Identity);
				NewActor->AttachToComponent(OwnedCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, ActorToSpawn.AttachName);

				if (SlotTag.MatchesTagExact(RPGInventoryTags::AttachPoint::RightHand))
				{
					OwnedCharacter->SetRightHandEquipment(NewActor);
				}
				else
				{
					OwnedCharacter->SetLeftHandEquipment(NewActor);
				}
				
				SpawnedActors.Emplace(NewActor);
			}
			else
			{
				Manager.RequestAsyncLoad(ActorToSpawn.EquipmentClass.ToSoftObjectPath(),
					[WeakThis,ActorToSpawn, this,SlotTag]
					{
						if (!WeakThis.IsValid())
							return;
						
						AEquipmentActor* NewActor = WeakThis->GetWorld()->SpawnActorDeferred<AEquipmentActor>(ActorToSpawn.EquipmentClass.Get(),
					FTransform::Identity, OwnedCharacter);
						NewActor->FinishSpawning(FTransform::Identity);
						NewActor->AttachToComponent(OwnedCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, ActorToSpawn.AttachName);

						if (SlotTag.MatchesTagExact(RPGInventoryTags::AttachPoint::RightHand))
						{
							OwnedCharacter->SetRightHandEquipment(NewActor);
						}
						else
						{
							OwnedCharacter->SetLeftHandEquipment(NewActor);
						}
						
						WeakThis->SpawnedActors.Emplace(NewActor);
					});
			}
		}
	}
}

void UEquipmentInstance::DestroySpawnedActors(FGameplayTag AttachTag)
{
	for (AActor* Actor : SpawnedActors)
	{
		Actor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		if (AttachTag.MatchesTagExact(RPGInventoryTags::AttachPoint::RightHand))
		{
			OwnedCharacter->RemoveRightHandEquipment();
		}
		else
		{
			OwnedCharacter->RemoveLeftHandEquipment();
		}
		Actor->Destroy();
	}
	SpawnedActors.Empty();
}

ACharacter* UEquipmentInstance::GetCharacter()
{
	if (ACharacter* Character = Cast<ACharacter>(GetOuter()))
	{
		return Character;
	}
	return nullptr;
}
