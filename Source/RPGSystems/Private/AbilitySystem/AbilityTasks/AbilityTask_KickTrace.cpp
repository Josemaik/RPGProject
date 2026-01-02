// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AbilityTask_KickTrace.h"

#include "Character/RPGSystemsCharacter.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetSystemLibrary.h"

class ARPGSystemsCharacter;

UAbilityTask_KickTrace* UAbilityTask_KickTrace::KickTrace(UGameplayAbility* OwningAbility, float SphereRadius)
{
	UAbilityTask_KickTrace* MyObj = NewAbilityTask<UAbilityTask_KickTrace>(OwningAbility);

	MyObj->Ability = OwningAbility;
	MyObj->SphereRadius = SphereRadius;

	return MyObj;
}

void UAbilityTask_KickTrace::Activate()
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,"KickTrace");
	if (!IsValid(Ability)) return;
	
	AActor* Avatar = Ability->GetAvatarActorFromActorInfo();
	if (!IsValid(Avatar)) return;
	ARPGSystemsCharacter* Character = Cast<ARPGSystemsCharacter>(Avatar);
	if (!IsValid(Character)) return;
	
	UArrowComponent* SphereTracePoint = Character->GetKickSphereTracePoint();
	if (!IsValid(SphereTracePoint)) return;

	FHitResult OutHit;
	FVector StartLocation = SphereTracePoint->GetComponentLocation();
	FVector EndLocation = StartLocation;
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwnerActor());
	
	bool bSphereHasHit = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		StartLocation,
		EndLocation,
		SphereRadius,
		TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		OutHit,
		true,
		FLinearColor::Blue,
		FLinearColor::Yellow,
		4.f);

	if (bSphereHasHit)
	{
		AActor* HitActor = OutHit.GetActor();
		if (!IsValid(HitActor)) return;
		
		OnHitActorDelegate.ExecuteIfBound(HitActor,OutHit.Location);
	}
	
	EndTask();
}

void UAbilityTask_KickTrace::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}
