// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Starting/TargetLockAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Animation/AnimInstanceProxy.h"
#include "Character/RPGSystemsCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UTargetLockAbility::UTargetLockAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UTargetLockAbility::CreateNewInputPressedEvent()
{
	WaitInputPressedTask = UAbilityTask_WaitInputPress::WaitInputPress(this, false);
	WaitInputPressedTask->OnPress.AddDynamic(this,&UTargetLockAbility::OnInputPressed);
	WaitInputPressedTask->ReadyForActivation();
}

void UTargetLockAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ActorInfo == nullptr)
	{
		return;
	}
	
	OwnedCharacter = Cast<ARPGSystemsCharacter>(ActorInfo->AvatarActor);
	
	CreateNewInputPressedEvent();

	bIsLocked = true;
	
	CalculateTargetLockedActor();
	OwnedCharacter->SetTargetLock(LockedActorFinded);
}

void UTargetLockAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (IsValid(WaitInputPressedTask))
	{
		WaitInputPressedTask->EndTask();
		WaitInputPressedTask = nullptr;
	}
}

void UTargetLockAbility::OnInputPressed(float TimeWaited)
{
	bIsLocked = !bIsLocked;

	if (bIsLocked)
	{
		CalculateTargetLockedActor();
		OwnedCharacter->SetTargetLock(LockedActorFinded);
	}
	else
	{
		OwnedCharacter->SetTargetLock(nullptr);
	}

	CreateNewInputPressedEvent();
}

void UTargetLockAbility::CalculateTargetLockedActor()
{
	FVector CameraLocation;
	FRotator CameraRotation;
	APlayerController* PC = Cast<APlayerController>(OwnedCharacter->GetController());
	if (!IsValid(PC)) return;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector TraceStart = CameraLocation;
	FVector TraceEnd = CameraLocation  + CameraRotation.Vector() * 1000.f;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add( UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnedCharacter);
	
	FHitResult HitResult;
	
	bool bHitResult = UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		TraceStart,
		TraceEnd,
		TraceRadius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.f
		);
	
	AActor* HitActor = HitResult.GetActor();
	if (bHitResult && IsValid(HitActor))
	{
		UAbilitySystemComponent* AbilitySystemComp = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
		if (!IsValid(AbilitySystemComp))
		{
			return;
		}
		FGameplayTag CharacterEnemyTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Character.Enemy")));
		if (AbilitySystemComp->GetOwnedGameplayTags().HasTag(CharacterEnemyTag))
		{
			LockedActorFinded = HitActor;
		}
	}
}
