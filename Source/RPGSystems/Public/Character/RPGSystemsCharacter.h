// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "CharacterBase.h"
#include "Components/TimelineComponent.h"
#include "Interfaces/RPGAbilitySystemInterface.h"
#include "Logging/LogMacros.h"
#include "RPGSystemsCharacter.generated.h"

struct FTimeline;
class UEquipmentInstance;
class URPGVaultComponent;
class IInteractableInterface;
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
class RPGSYSTEMS_API ARPGSystemsCharacter : public ACharacterBase,public IAbilitySystemInterface, public IRPGAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ARPGSystemsCharacter(const FObjectInitializer& ObjectInitializer);
	
	/*Inventory and Equipment Getters*/
	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	UEquipmentManagerComponent* GetEquipmentComponent() const { return EquipmentComponent; }
	
	/*equipment instances*/
	void SetEquipment(UEquipmentInstance* NewInstance);
	void RemoveEquipment(UEquipmentInstance* InstanceToRemove);
	void ChangueEquipmentAttachPoint(FGameplayTag OldAttachTag,FGameplayTag NewAttachTag);
	
	AEquipmentActor* GetEquipmentActor(FGameplayTag AttachTag);
	TSoftClassPtr<AEquipmentActor> GetEquipmentClassBySlotTag(FGameplayTag SlotTag) const;

	/*Camera Aiming*/
	void OnRangeStartAiming();
	void OnRangeStopAiming();

	/*Combat*/
	UArrowComponent* GetKickSphereTracePoint() const { return KickSphereTracePoint; }
	TMap<FGameplayTag,FName>& GetSocketNames() { return AttachNames; }
	void UpdateAttackWarpTarget() const;

	//character capture for UI
	void AddEquipmentToCharacterCapture(AActor* Actor) const;
	void RemoveEquipmentFromCharacterCapture(AActor* Actor) const;

	UFUNCTION(BlueprintCallable)
	void Test();
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	//replication
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	//Input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	virtual void Jump() override;
	virtual void StopJumping() override;
	//controller input
	void Look(const FInputActionValue& Value);
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/*Ability System Interface*/
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	/*RPG Ability System Interface*/
	virtual USceneComponent* GetDynamicSpawnPoint_Implementation() override;
	virtual void AddToExperience_Implementation(const FScalableFloat& XPScale) override;
	
	virtual void InitAbilityActorInfo() override;
	virtual void BindCallbacksToDependencies() override;
	virtual void InitClassDefaults() override;
	virtual void BroadcastInitialValues() override;

	//Interact
	void Interact();
	UPROPERTY(BlueprintReadWrite)
	TScriptInterface<IInteractableInterface> ThisFrameInteractActor;
private:
	UFUNCTION()
	void UpdateCameraAim(float Value);

	UFUNCTION()
	void OnRep_InventoryComponent();

	UFUNCTION()
	void OnRep_EquipmentComponent();

	virtual void Death_Implementation() override;
	
	//Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	//Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;
	
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

	// Gameplay Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Motion, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URPGMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CaptureCharacter, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CharacterCaptureSpringArm;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CaptureCharacter, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneCaptureComponent2D> CharacterCaptureComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Motion, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URPGVaultComponent> VaultComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_InventoryComponent)
	TObjectPtr<UInventoryComponent> InventoryComponent;
	
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_EquipmentComponent)
	TObjectPtr<UEquipmentManagerComponent> EquipmentComponent;

	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess=true))
	TObjectPtr<USceneComponent> DynamicProjectileSpawnPoint;

	UPROPERTY(EditDefaultsOnly, Category=Scene, meta=(AllowPrivateAccess=true))
	TObjectPtr<UArrowComponent> KickSphereTracePoint;
	
	UPROPERTY(BlueprintReadOnly,meta =(AllowPrivateAccess=true))
	TObjectPtr<URPGAbilitySystemComponent> RPGAbilitySystemComp;

	UPROPERTY(BlueprintReadOnly,meta =(AllowPrivateAccess=true))
	TObjectPtr<URPGAttributeSet> RPGAttributes;

	//Equipments instances 
	TArray<UEquipmentInstance*> EquipmentInstances;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag,FName> AttachNames;
	
	//Camera Aiming
	FTimeline CameraTimeline;

	UPROPERTY(EditAnywhere, Category = "Camera|Aim")
	UCurveFloat* CameraAimCurve;
	
	float DefaultArmLength = 0.f;
	FVector DefaultSocketOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Camera|Aim")
	float AimArmLength = 300.f;
	
	UPROPERTY(EditAnywhere, Category = "Camera|Aim")
	FVector AimSocketOffset = FVector(149.f, 43.f, 73.f);

	UPROPERTY(EditAnywhere, Category = "Camera|Aim")
	float MaxSpeedWhileAiming = 130.f;
	float DefaultMaxSpeed;

	UFUNCTION(BlueprintCallable)
	void SetTargetLock(AActor* NewTargetLockActor);
	
	//Combat
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Combat", meta=(allowPrivateAccess="true"))
	TObjectPtr<AActor> TargetLock;
};


