// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentInstance.h"


#include "Character/RPGSystemsCharacter.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Equipment/EquipmentActors/EquipmentActor.h"
#include "Equipment/EquipmentDefinition.h"
#include "GameFramework/Character.h"

void UEquipmentInstance::OnEquipped()
{
}

void UEquipmentInstance::OnUnEquipped()
{
}

void UEquipmentInstance::SpawnEquipmentActors(const TArray<FEquipmentActorsToSpawn>& ActorsToSpawn,FGameplayTag AttachTag)
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

				OwnedCharacter->AttachNames.Add(AttachTag,ActorToSpawn.AttachName);
				SpawnedActors.Emplace(AttachTag,NewActor);
				OwnedCharacter->SetEquipment(this);
			}
			else
			{
				Manager.RequestAsyncLoad(ActorToSpawn.EquipmentClass.ToSoftObjectPath(),
					[WeakThis,ActorToSpawn, this,AttachTag]
					{
						if (!WeakThis.IsValid())
							return;
						
						AEquipmentActor* NewActor = WeakThis->GetWorld()->SpawnActorDeferred<AEquipmentActor>(ActorToSpawn.EquipmentClass.Get(),
					FTransform::Identity, OwnedCharacter);
						NewActor->FinishSpawning(FTransform::Identity);
						NewActor->AttachToComponent(OwnedCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, ActorToSpawn.AttachName);

						OwnedCharacter->AttachNames.Add(AttachTag,ActorToSpawn.AttachName);
						WeakThis->SpawnedActors.Emplace(AttachTag,NewActor);
						OwnedCharacter->SetEquipment(this);
					});
			}
		}
	}
}

void UEquipmentInstance::DestroySpawnedActors(FGameplayTag AttachTag)
{
	for (const TPair<FGameplayTag,AEquipmentActor*>& Actor : SpawnedActors)
	{
		Actor.Value->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		OwnedCharacter->RemoveEquipment(this);
		Actor.Value->Destroy();
	}
	SpawnedActors.Empty();
}

void UEquipmentInstance::ChangeAttachPoint(FGameplayTag OldAttachTag,FGameplayTag NewAttachTag)
{
	GEngine->AddOnScreenDebugMessage(-1,4.f,FColor::Red,"AttachPoint");
	AEquipmentActor* AttachedActor = GetActorAttached(OldAttachTag);
	if (IsValid(AttachedActor))
	{
		AttachedActor->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		
		FName SocketName = *OwnedCharacter->AttachNames.Find(NewAttachTag);
		AttachedActor->AttachToComponent(OwnedCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketName);
		SpawnedActors.Remove(OldAttachTag);
		SpawnedActors.Add(NewAttachTag,AttachedActor);
	}
}

AEquipmentActor* UEquipmentInstance::GetActorAttached(FGameplayTag AttachPoint)
{
	if (AEquipmentActor** FoundActor = SpawnedActors.Find(AttachPoint))
	{
		if (IsValid(*FoundActor))
		{
			return *FoundActor;
		}
	}
	return nullptr;
}

bool UEquipmentInstance::HasAnActorAttached(FGameplayTag AttachPoint) const
{
	if (SpawnedActors.Contains(AttachPoint))
	{
		return true;
	}
	return false;
}


ACharacter* UEquipmentInstance::GetCharacter()
{
	if (ACharacter* Character = Cast<ACharacter>(GetOuter()))
	{
		return Character;
	}
	return nullptr;
}
