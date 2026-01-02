// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AbilityTask_SwordTrace.h"

#include "Character/RPGSystemsCharacter.h"
#include "Components/ArrowComponent.h"
#include "Equipment/EquipmentActor.h"
#include "Equipment/SwordActor.h"
#include "Kismet/KismetSystemLibrary.h"

void UAbilityTask_SwordTrace::Activate()
{
	if (!IsValid(Ability)) return;
	
	AActor* Avatar = Ability->GetAvatarActorFromActorInfo();
	if (!IsValid(Avatar)) return;
	
	ARPGSystemsCharacter* Character = Cast<ARPGSystemsCharacter>(Avatar);
	if (!IsValid(Character)) return;
	
	ASwordActor* Sword = Cast<ASwordActor>(Character->GetRightHandEquipmentActor());
	if (!IsValid(Sword)) return;
	
	TopPointArrow = Cast<UArrowComponent>(Sword->GetSwordTopPoint());
	ButtomPointArrow = Cast<UArrowComponent>(Sword->GetSwordButtomPoint());
	
	TraceCheckDelegate.BindUObject(this,&UAbilityTask_SwordTrace::TraceCheck);
	GetWorld()->GetTimerManager().SetTimer(TraceCheckTimer,TraceCheckDelegate,0.001f,true);
}

void UAbilityTask_SwordTrace::OnDestroy(bool bInOwnerFinished)
{
	TraceCheckDelegate.Unbind();
	GetWorld()->GetTimerManager().ClearTimer(TraceCheckTimer);
	HitActors.Empty();
	Super::OnDestroy(bInOwnerFinished);
}

UAbilityTask_SwordTrace* UAbilityTask_SwordTrace::ShordTrace(UGameplayAbility* OwningAbility,float SphereRadius)
{
	UAbilityTask_SwordTrace* MyObj = NewAbilityTask<UAbilityTask_SwordTrace>(OwningAbility);

	MyObj->Ability = OwningAbility;
	MyObj->SphereRadius = SphereRadius;

	return MyObj;
}

void UAbilityTask_SwordTrace::TraceCheck()
{
	if (!IsValid(TopPointArrow) || !IsValid(ButtomPointArrow)) return;
	
	FHitResult OutHit;
	FVector StartLocation = TopPointArrow->GetComponentLocation();
	FVector EndLocation = ButtomPointArrow->GetComponentLocation();
	
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
		FLinearColor::Red,
		FLinearColor::Green,
		0.1f);
	
	if (bSphereHasHit)
	{
		AActor* HitActor = OutHit.GetActor();
		if (!IsValid(HitActor) || HitActors.Contains(HitActor)) return;

		HitActors.Add(HitActor);
		OnHitActorDelegate.ExecuteIfBound(HitActor,OutHit.Location);
	}
}
