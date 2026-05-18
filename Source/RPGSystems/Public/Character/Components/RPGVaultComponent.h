// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGVaultComponent.generated.h"


class UMotionWarpingComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RPGSYSTEMS_API URPGVaultComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URPGVaultComponent();
	virtual void BeginPlay() override;

	void SetMotionWarpingReference(const TObjectPtr<UMotionWarpingComponent>& InMotionWarpingComponent);
	
	//Vault
	UFUNCTION(BlueprintCallable)
	void TryVault();

	bool GetCanVault() const { return CanVault; };

private:
	void VaultMotionWarp();

	UFUNCTION()
	void OnVaultCompleted(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY()
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"),Category="Vault")
	UAnimMontage* VaultMontage;

	const float VaultZOffsetFirstTrace = 30.f;
	const float VaultZOffsetSecondTrace = 50.f;
	const int32 NumTracesFirstCheck = 3;
	const int32 NumTracesSecondCheck = 6;
	const float MaxDetectionDistance = 180.f;
	const float LandingZOffset = 50.f;
	const FVector ZOffsetVector = FVector(0, 0, 100.f);
	
	FVector VaultStartPos = FVector::ZeroVector;
	FVector VaultMiddlePos = FVector::ZeroVector;
	FVector VaultLandPos = FVector::ZeroVector;

	bool CanVault = false;
	
	UPROPERTY(EditDefaultsOnly, Category="Vault")
	float VaultSphereRadiusFirstCheck = 5.f;
	UPROPERTY(EditDefaultsOnly, Category="Vault")
	float VaultSphereRadiusSecondCheck = 10.f;
};
