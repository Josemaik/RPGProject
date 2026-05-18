// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/RPGVaultComponent.h"

#include "Character/Components/RPGMotionWarpingComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values for this component's properties
URPGVaultComponent::URPGVaultComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void URPGVaultComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ACharacter>(GetOwner());
}

void URPGVaultComponent::SetMotionWarpingReference(const TObjectPtr<UMotionWarpingComponent>& InMotionWarpingComponent)
{
	MotionWarpingComponent = InMotionWarpingComponent;
}

void URPGVaultComponent::TryVault()
{
	if (!IsValid(OwnerCharacter))
	{
		return;
	}
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	FHitResult OutHitFirstCheck;
	for (int i = 0; i < NumTracesFirstCheck;i++)
	{
		
		FVector StartLocation = OwnerCharacter->GetActorLocation() + FVector(0, 0, i * VaultZOffsetFirstTrace);
		FVector EndLocation = StartLocation + OwnerCharacter->GetActorForwardVector() * MaxDetectionDistance;
		
		bool bShereHit = UKismetSystemLibrary::SphereTraceSingle(
			this,
			StartLocation,
			EndLocation,
			VaultSphereRadiusFirstCheck,
			TraceTypeQuery1,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			OutHitFirstCheck,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			5.f
			);
		
		if (bShereHit)
		{
			break;
		}
	}
		
	for (int j = 0; j < NumTracesSecondCheck;j++)
	{
		FHitResult OutHit;
		FVector ForwardOffset = OwnerCharacter->GetActorForwardVector() * (j * VaultZOffsetSecondTrace);
		// GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,
		// 	FString::Printf(TEXT("checks: %d"),j));
		FVector HitLocationZOffset = OutHitFirstCheck.Location + ZOffsetVector;
		FVector StartLocationSecondCheck = HitLocationZOffset + ForwardOffset;
		FVector EndLocationSecondCheck = StartLocationSecondCheck - ZOffsetVector;
		
		bool bSecondCheckHit = UKismetSystemLibrary::SphereTraceSingle(
			this,
			StartLocationSecondCheck,
			EndLocationSecondCheck,
			VaultSphereRadiusSecondCheck,
			TraceTypeQuery1,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			OutHit,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			5.f
			);
		if (bSecondCheckHit)
		{
			if (OutHit.bStartPenetrating)
			{
				CanVault = false;
				VaultLandPos = FVector(0, 0, 20000);
				break;
			}
			
			if (j == 0)
			{
				VaultStartPos = OutHit.Location;
				UKismetSystemLibrary::DrawDebugSphere(
				this,VaultStartPos,VaultSphereRadiusSecondCheck,12,
				FColor::Purple,10.f,2.f);
			}
			VaultMiddlePos = OutHit.Location;
			UKismetSystemLibrary::DrawDebugSphere(
			this,VaultMiddlePos,VaultSphereRadiusSecondCheck,12,
			FColor::Yellow,10.f,2.f);
		}
		else
		{
			FHitResult OutHitThirdCheck;
			FVector DirectionWithOffset = OwnerCharacter->GetActorForwardVector() * 80.f;
			FVector StartLocationThirdCheck = OutHit.TraceStart + DirectionWithOffset;
			FVector EndLocationThirdCheck = StartLocationThirdCheck - FVector(0,0,1000);
			bool bLineTraceHit = UKismetSystemLibrary::LineTraceSingle(this,StartLocationThirdCheck,EndLocationThirdCheck,
				TraceTypeQuery1,false,ActorsToIgnore,EDrawDebugTrace::None,OutHitThirdCheck,
				true,FColor::Blue,FColor::Red,5.f);
			if (bLineTraceHit)
			{
				VaultLandPos = OutHitThirdCheck.Location;
				CanVault = true;
				VaultMotionWarp();
				break;
			}
		}
	}
}

void URPGVaultComponent::VaultMotionWarp()
{
	if (!IsValid(OwnerCharacter) || !IsValid(MotionWarpingComponent))
	{
		return;
	}
	
	USkeletalMeshComponent* MeshRef = OwnerCharacter->GetMesh();
	if (!IsValid(MeshRef))
	{
		return;
	}
	
	float MeshZWorldLocation = MeshRef->GetComponentLocation().Z;
	float MinZOffset = MeshZWorldLocation - LandingZOffset;
	float MaxZOffset = MeshZWorldLocation + LandingZOffset;
	
	if (CanVault && VaultLandPos.Z >=  MinZOffset && VaultLandPos.Z <= MaxZOffset)
	{
		UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
		if (!IsValid(AnimInstance) || !IsValid(VaultMontage))
		{
			return;
		}
		
		OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		OwnerCharacter->SetActorEnableCollision(false);
		
		FMotionWarpingTarget TargetStart;
		TargetStart.Location = VaultStartPos;
		TargetStart.Name = FName("VaultStart");
		TargetStart.Rotation = OwnerCharacter->GetActorRotation();
		MotionWarpingComponent->AddOrUpdateWarpTarget(TargetStart);
		FMotionWarpingTarget TargetMiddle;
		TargetMiddle.Location = VaultMiddlePos;
		TargetMiddle.Name = FName("VaultMiddle");
		TargetMiddle.Rotation = OwnerCharacter->GetActorRotation();
		MotionWarpingComponent->AddOrUpdateWarpTarget(TargetMiddle);
		FMotionWarpingTarget TargetLand;
		TargetLand.Location = VaultLandPos;
		TargetLand.Name = FName("VaultLand");
		TargetLand.Rotation = OwnerCharacter->GetActorRotation();
		MotionWarpingComponent->AddOrUpdateWarpTarget(TargetLand);

		AnimInstance->OnMontageEnded.AddDynamic(this, &URPGVaultComponent::OnVaultCompleted);
		AnimInstance->Montage_Play(VaultMontage);
	}
}

void URPGVaultComponent::OnVaultCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (!IsValid(OwnerCharacter))
	{
		return;
	}
	
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	OwnerCharacter->SetActorEnableCollision(true);
	CanVault = false;
	VaultLandPos = FVector(0, 0, 20000);
	
	OwnerCharacter->GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &URPGVaultComponent::OnVaultCompleted);
}

