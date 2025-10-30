// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ProjectileAbility.h"

#include "Data/ProjectileInfo.h"
#include "Interfaces/RPGAbilitySystemInterface.h"
#include "Libraries/RPGAbilitySystemLibrary.h"
#include "Projectiles/ProjectileBase.h"
#include "Tests/AutomationCommon.h"

UProjectileAbility::UProjectileAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UProjectileAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	AvatarActorFromInfo = GetAvatarActorFromActorInfo();
	
	if (!ProjectileToSpawnTag.IsValid() || !IsValid(AvatarActorFromInfo))
	{
		return;
	}

	if (UProjectileInfo* ProjectileInfo = URPGAbilitySystemLibrary::GetProjectileInfo(GetAvatarActorFromActorInfo()))
	{
		CurrentProjectileParams = *ProjectileInfo->ProjectileInfoMap.Find(ProjectileToSpawnTag);	
	}
}

void UProjectileAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthority(&ActivationInfo))
	{
		SpawnProjectile();
	}
	
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &UProjectileAbility::EndAbility, Handle, ActorInfo, ActivationInfo, false, false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,TimerDelegate, 0.25f, false);
	
}

void UProjectileAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UProjectileAbility::SpawnProjectile()
{
	if (!IsValid(CurrentProjectileParams.ProjectileClass))
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
		FString::Printf(TEXT("Spawning Projectile %s On Server"), *CurrentProjectileParams.ProjectileClass->GetName()));

	if (const USceneComponent* SpawnPointComp = IRPGAbilitySystemInterface::Execute_GetDynamicSpawnPoint(AvatarActorFromInfo))
	{
		const FVector SpawnPoint = SpawnPointComp->GetComponentLocation();
		const FVector TargetLocation = AvatarActorFromInfo->GetActorForwardVector() * 10000.f;
		const FRotator TargetRotation = (TargetLocation - SpawnPoint).Rotation();

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnPoint);
		SpawnTransform.SetRotation(TargetRotation.Quaternion());
		
		if (AProjectileBase* SpawnedProjectile = GetWorld()->SpawnActorDeferred<AProjectileBase>(CurrentProjectileParams.ProjectileClass, SpawnTransform))
		{
			SpawnedProjectile->SetProjectileParams(CurrentProjectileParams);
			SpawnedProjectile->FinishSpawning(SpawnTransform);
		}
	}
}
