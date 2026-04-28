// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "CharacterBase.h"
#include "Interfaces/RPGAbilitySystemInterface.h"
#include "Logging/LogMacros.h"
#include "RPGSystemsCharacter.generated.h"

class UEquipmentManagerComponent;
class UInventoryComponent;
class URPGMotionWarpingComponent;
class URPGAttributeSet;
class URPGAbilitySystemComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AEquipmentActor;
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* VaultAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AssasinationAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Motion, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URPGMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CaptureCharacter, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CharacterCaptureSpringArm;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CaptureCharacter, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneCaptureComponent2D> CharacterCaptureComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_InventoryComponent)
	TObjectPtr<UInventoryComponent> InventoryComponent;
	
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_EquipmentComponent)
	TObjectPtr<UEquipmentManagerComponent> EquipmentComponent;

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
	/*Inventory and Equipment Getters*/
	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	UEquipmentManagerComponent* GetEquipmentComponent() const { return EquipmentComponent; }
	
	/*equipment instances*/
	AEquipmentActor* GetRightHandEquipmentActor() const { return RightHandEquipment; }
	AEquipmentActor* GetLeftHandEquipmentActor() const { return LeftHandEquipment; }
	void SetRightHandEquipment(AEquipmentActor* NewRightHandEquipment);
	void SetLeftHandEquipment(AEquipmentActor* NewLeftHandEquipment);
	void RemoveRightHandEquipment();
	void RemoveLeftHandEquipment();

	UArrowComponent* GetKickSphereTracePoint() const { return KickSphereTracePoint; }
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	void Jump() override;
	void StopJumping() override;

	/*Called for sprint input*/
	void Sprint();
	
	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	virtual void InitAbilityActorInfo() override;
	virtual void BindCallbacksToDependencies() override;
	virtual void InitClassDefaults() override;
	virtual void BroadcastInitialValues() override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void AddEquipmentToCharacterCapture(AActor* Actor) const;
	void RemoveEquipmentFromCharacterCapture(AActor* Actor) const;
private:
	UFUNCTION()
	void OnRep_InventoryComponent();

	UFUNCTION()
	void OnRep_EquipmentComponent();

	/*Move to Component***********************/
	UFUNCTION(BlueprintCallable)
	void TryVault();

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

	bool CanVault = false;
	
	UPROPERTY()
	float VaultSphereRadiusFirstCheck;
	UPROPERTY()
	float VaultSphereRadiusSecondCheck;
	/****************************************/

	UPROPERTY()
	AEquipmentActor* RightHandEquipment;

	UPROPERTY()
	AEquipmentActor* LeftHandEquipment;


	virtual void Death_Implementation() override;
	
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess=true))
	TObjectPtr<USceneComponent> DynamicProjectileSpawnPoint;

	UPROPERTY(EditDefaultsOnly, Category=Scene, meta=(AllowPrivateAccess=true))
	TObjectPtr<UArrowComponent> KickSphereTracePoint;
	
	UPROPERTY(BlueprintReadOnly,meta =(AllowPrivateAccess=true))
	TObjectPtr<URPGAbilitySystemComponent> RPGAbilitySystemComp;

	UPROPERTY(BlueprintReadOnly,meta =(AllowPrivateAccess=true))
	TObjectPtr<URPGAttributeSet> RPGAttributes;
};