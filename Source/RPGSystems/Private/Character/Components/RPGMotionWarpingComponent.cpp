// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/RPGMotionWarpingComponent.h"
#include "MotionWarpingAdapter.h"
#include "Character/EnemyBase.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void URPGMotionWarpingComponent::SetLockedTarget(AActor* NewLockedTarget)
{
	LockedTarget = NewLockedTarget;
}

void URPGMotionWarpingComponent::UpdateAttackWarpTarget()
{
	//is Target has value -> locked
	if (IsValid(LockedTarget))
	{
		UMotionWarpingBaseAdapter* MotionWarpingBaseAdapter = GetOwnerAdapter();
		if (!IsValid(MotionWarpingBaseAdapter)) return;
	
		ACharacter* OwnerCharacter = Cast<ACharacter>(MotionWarpingBaseAdapter->GetActor());
		if (!IsValid(OwnerCharacter))
		{
			return;
		}
		
		FVector DirectionToPlayer =
		(
			OwnerCharacter->GetActorLocation() -
			LockedTarget->GetActorLocation()
		).GetSafeNormal();
		
		FVector WarpLocation = LockedTarget->GetActorLocation() + DirectionToPlayer * 120.f;
		FRotator WarpRotation = UKismetMathLibrary::FindLookAtRotation(
			OwnerCharacter->GetActorLocation(),
			LockedTarget->GetActorLocation()
		);

		FTransform WarpTransform;
		WarpTransform.SetLocation(WarpLocation);
		WarpTransform.SetRotation(WarpRotation.Quaternion());
		
		AddOrUpdateWarpTargetFromTransform(
			FName("AttackTarget"),
			WarpTransform
		);
		return;
	}

	LockedTarget = nullptr;

	RemoveWarpTarget(FName("AttackTarget"));

	//searh nearest target
	FindAndSetNearestEnemyTarget();
}

void URPGMotionWarpingComponent::FindAndSetNearestEnemyTarget()
{
	UMotionWarpingBaseAdapter* MotionWarpingBaseAdapter = GetOwnerAdapter();
	if (!IsValid(MotionWarpingBaseAdapter)) return;
	
	ACharacter* OwnerCharacter = Cast<ACharacter>(MotionWarpingBaseAdapter->GetActor());
	if (!IsValid(OwnerCharacter))
	{
		return;
	}
	
	const float ConeAngle = 90.f;   // angles
	const float ConeRange = 300.f;  // max distance

	TArray<AActor*> OverlappingActors;
	const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes { EObjectTypeQuery::ObjectTypeQuery1 };
	// Sphere overlap 
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(), OwnerCharacter->GetActorLocation(), ConeRange,
		ObjectTypes, AEnemyBase::StaticClass(),
		{OwnerCharacter}, OverlappingActors
	);

	AActor* BestTarget = nullptr;
	float BestScore = FLT_MAX;

	for (AActor* Actor : OverlappingActors)
	{
		FVector ToActor = (Actor->GetActorLocation() - OwnerCharacter->GetActorLocation());
		float Angle = FMath::RadiansToDegrees(
			FMath::Acos(FVector::DotProduct(OwnerCharacter->GetActorForwardVector(), ToActor.GetSafeNormal()))
		);

		//Filter
		if (Angle > ConeAngle * 0.5f) continue;

		float Distance = ToActor.Size();
		if (Distance < BestScore)
		{
			BestScore = Distance;
			BestTarget = Actor;
		}
	}

	if (IsValid(BestTarget))
	{
		AddOrUpdateWarpTargetFromTransform(
			FName("AttackTarget"),
			BestTarget->GetActorTransform()
		);
	}
	else
	{
		RemoveWarpTarget(FName("AttackTarget"));
	}
}
