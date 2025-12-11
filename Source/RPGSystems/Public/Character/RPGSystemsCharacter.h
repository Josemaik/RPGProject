// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "CharacterBase.h"
#include "Interfaces/RPGAbilitySystemInterface.h"
#include "Logging/LogMacros.h"
#include "RPGSystemsCharacter.generated.h"

class URPGMotionWarpingComponent;
class URPGAttributeSet;
class URPGAbilitySystemComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ARPGSystemsCharacter : public ACharacterBase,public IAbilitySystemInterface, public IRPGAbilitySystemInterface
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Motion, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URPGMotionWarpingComponent> MotionWarpingComponent;

public:
	ARPGSystemsCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/*Ability System Interface*/
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	/*RPG Ability System Interface*/
	virtual USceneComponent* GetDynamicSpawnPoint_Implementation() override;
	virtual void AddToExperience_Implementation(const FScalableFloat& XPScale) override;
	
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	virtual void InitAbilityActorInfo() override;
	virtual void BindCallbacksToDependencies() override;
	virtual void InitClassDefaults() override;
	virtual void BroadcastInitialValues() override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	
private:
	
	UFUNCTION(BlueprintCallable)
	void Vault();

	void VaultMotionWarp();

	UFUNCTION()
	void OnVaultCompleted(UAnimMontage* Montage, bool bInterrupted);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"),Category="Custom Values|Animation")
	UAnimMontage* VaultMontage;
	
	//Vaulting
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

	bool CanWarp = false;
	
	UPROPERTY()
	float VaultSphereRadiusFirstCheck;
	UPROPERTY()
	float VaultSphereRadiusSecondCheck;
	
	
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess=true))
	TObjectPtr<USceneComponent> DynamicProjectileSpawnPoint;
	
	UPROPERTY(BlueprintReadOnly,meta =(AllowPrivateAccess=true))
	TObjectPtr<URPGAbilitySystemComponent> RPGAbilitySystemComp;

	UPROPERTY(BlueprintReadOnly,meta =(AllowPrivateAccess=true))
	TObjectPtr<URPGAttributeSet> RPGAttributes;
};

