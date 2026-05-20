// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/RPGSystemsCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/RPGAttributeSet.h"
#include "AbilitySystem/NativeTags/RPGGameplayTags.h"
#include "AbilitySystem/NativeTags/RPGInventoryTags.h"
#include "Character/Animation/RPGAnimInstance.h"
#include "Character/Components/RPGCharacterMovement.h"
#include "Data/CharacterClassInfo.h"
#include "Game/PlayerState/RPGPlayerState.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Libraries/RPGAbilitySystemLibrary.h"
#include "Character/Components/RPGMotionWarpingComponent.h"
#include "Character/Components/RPGVaultComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/TimelineComponent.h"
#include "Equipment/EquipmentInstance.h"
#include "Equipment/EquipmentActors/EquipmentActor.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "Interfaces/InteractableInterface.h"
#include "InventorySection/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ARPGSystemsCharacter

ARPGSystemsCharacter::ARPGSystemsCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<URPGCharacterMovement>(CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	DynamicProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("DynamicProjectileSpawnPoint"));
	DynamicProjectileSpawnPoint->SetupAttachment(GetRootComponent());
	KickSphereTracePoint = CreateDefaultSubobject<UArrowComponent>(TEXT("KickSphereTrace"));
	KickSphereTracePoint->SetupAttachment(GetRootComponent());

	MotionWarpingComponent = CreateDefaultSubobject<URPGMotionWarpingComponent>(TEXT("MotionWarpingComponent"));

	VaultComponent = CreateDefaultSubobject<URPGVaultComponent>(TEXT("VaultComponent"));
	VaultComponent->SetMotionWarpingReference(MotionWarpingComponent);
	
	CharacterCaptureSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CharacterCaptureSpringArm"));
	CharacterCaptureSpringArm->SetupAttachment(GetCapsuleComponent());

	CharacterCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CharacterCaptureComponent"));
	CharacterCaptureComponent->SetupAttachment(CharacterCaptureSpringArm);

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetIsReplicated(true);
	
	EquipmentComponent = CreateDefaultSubobject<UEquipmentManagerComponent>(TEXT("EquipmentManagerComponent"));
	EquipmentComponent->SetIsReplicated(true);
	
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void ARPGSystemsCharacter::PossessedBy(AController* NewController) //server
{
	Super::PossessedBy(NewController);

	if (HasAuthority())
	{
		InitAbilityActorInfo();
	}
}

void ARPGSystemsCharacter::OnRep_PlayerState() //clients
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo(); 
}

UAbilitySystemComponent* ARPGSystemsCharacter::GetAbilitySystemComponent() const
{
	return RPGAbilitySystemComp;
}

USceneComponent* ARPGSystemsCharacter::GetDynamicSpawnPoint_Implementation()
{
	return DynamicProjectileSpawnPoint;
}

void ARPGSystemsCharacter::AddToExperience_Implementation(const FScalableFloat& XPScale)
{
	if (ARPGPlayerState* RPGPlayerState = GetPlayerState<ARPGPlayerState>())
	{
		RPGPlayerState->AddToExperience(XPScale);
	}
}

TSoftClassPtr<AEquipmentActor> ARPGSystemsCharacter::GetEquipmentClassBySlotTag(FGameplayTag SlotTag) const
{
	return EquipmentComponent->GetEquipmentClassBySlotTag(SlotTag);
}

void ARPGSystemsCharacter::UpdateCameraAim(float Value)
{
	CameraBoom->TargetArmLength = FMath::Lerp(DefaultArmLength, AimArmLength, Value);
	CameraBoom->SocketOffset = FMath::Lerp(DefaultSocketOffset, AimSocketOffset, Value);
}

void ARPGSystemsCharacter::OnRangeStartAiming()
{
	TObjectPtr<UCharacterMovementComponent> CharacterMovementComp = GetCharacterMovement();
	if (!IsValid(CharacterMovementComp)) { return; }
	
	if (!IsValid(CameraBoom)) { return; }

	//Save default values
	DefaultMaxSpeed = CharacterMovementComp->GetMaxSpeed();
	DefaultArmLength = CameraBoom->TargetArmLength;
	DefaultSocketOffset = CameraBoom->SocketOffset;
	
	CharacterMovementComp->MaxWalkSpeed = MaxSpeedWhileAiming;
	CharacterMovementComp->bOrientRotationToMovement = false;
	CharacterMovementComp->bUseControllerDesiredRotation = true;
	
	CameraTimeline.Play();
}

void ARPGSystemsCharacter::OnRangeStopAiming()
{
	TObjectPtr<UCharacterMovementComponent> CharacterMovementComp = GetCharacterMovement();
	if (!IsValid(CharacterMovementComp)) { return; }
	
	if (!IsValid(CameraBoom)) { return; }
	
	//Restore Default values
	CharacterMovementComp->MaxWalkSpeed = DefaultMaxSpeed;
	CharacterMovementComp->bOrientRotationToMovement = true;
	CharacterMovementComp->bUseControllerDesiredRotation = false;
	
	CameraTimeline.Reverse();
}

void ARPGSystemsCharacter::InitAbilityActorInfo()
{
	ARPGPlayerState* RPGPlayerState = GetPlayerState<ARPGPlayerState>();
	if (!IsValid(RPGPlayerState))
	{
		return;
	}

	RPGAbilitySystemComp = RPGPlayerState->GetRPGAbilitySystemComponent();
	RPGAttributes = RPGPlayerState->GetRPGAttributeSet();

	if (!IsValid(RPGAbilitySystemComp) || !IsValid(RPGAttributes))
	{
		return;
	}

	RPGAbilitySystemComp->InitAbilityActorInfo(RPGPlayerState,this);

	BindCallbacksToDependencies();
	
	if (HasAuthority())
	{
		InitClassDefaults();
	}

	if (URPGAnimInstance* RPGAnimInstance = Cast<URPGAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		RPGAnimInstance->InitializeWithAbilitySystem(RPGAbilitySystemComp);
	}
}

void ARPGSystemsCharacter::InitClassDefaults()
{
	if (!CharacterTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Character Tag Selected in this Character %s"), *GetNameSafe(this));
		return;
	}

	UCharacterClassInfo* ClassInfo = URPGAbilitySystemLibrary::GetCharacterClassDefaultInfo(this);
	if (!IsValid(ClassInfo))
	{
		return;
	}

	const FCharacterClassDefaultInfo* SelectedClassInfo =  ClassInfo->ClassDefaultInfoMap.Find(CharacterTag);
	if (!SelectedClassInfo || !IsValid(RPGAbilitySystemComp))
	{
		return;
	}
	
	RPGAbilitySystemComp->AddCharacterAbilities(SelectedClassInfo->StartingAbilities);
	RPGAbilitySystemComp->AddCharacterPassiveAbilities(SelectedClassInfo->StartingPassives);
	RPGAbilitySystemComp->InitializeDefaultAttributes(SelectedClassInfo->DefaultAttributes);
	RPGAbilitySystemComp->InitializeDefaultInfiniteEffects(SelectedClassInfo->StartingInfiniteEffects);
}

void ARPGSystemsCharacter::BindCallbacksToDependencies()
{
	if (!IsValid(RPGAbilitySystemComp) || !IsValid(RPGAttributes))
	{
		return;
	}

	RPGAbilitySystemComp->GetGameplayAttributeValueChangeDelegate(RPGAttributes->GetHealthAttribute()).AddLambda(
		[this] (const FOnAttributeChangeData& Data)
		{
			OnHealthChanged(Data.NewValue, RPGAttributes->GetMaxHealth());
		});

	RPGAbilitySystemComp->GetGameplayAttributeValueChangeDelegate(RPGAttributes->GetManaAttribute()).AddLambda(
		[this] (const FOnAttributeChangeData& Data)
		{
			OnManaChanged(Data.NewValue, RPGAttributes->GetMaxMana());
		});

	RPGAbilitySystemComp->GetGameplayAttributeValueChangeDelegate(RPGAttributes->GetShieldAttribute()).AddLambda(
		[this] (const FOnAttributeChangeData& Data)
		{
			OnShieldChanged(Data.NewValue, RPGAttributes->GetMaxShield());
		});
	RPGAbilitySystemComp->GetGameplayAttributeValueChangeDelegate(RPGAttributes->GetMovementSpeedAttribute()).AddLambda(
		[this] (const FOnAttributeChangeData& Data)
		{
			GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
		});
	RPGAbilitySystemComp->GetGameplayAttributeValueChangeDelegate(RPGAttributes->GetStaminaAttribute()).AddLambda(
		[this] (const FOnAttributeChangeData& Data)
		{
			OnStaminaChanged(Data.NewValue, RPGAttributes->GetMaxStamina());
		});
}

void ARPGSystemsCharacter::BroadcastInitialValues()
{
	if (!IsValid(RPGAttributes))
	{
		return;
	}

	OnHealthChanged(RPGAttributes->GetHealth(), RPGAttributes->GetMaxHealth());
	OnManaChanged(RPGAttributes->GetMana(), RPGAttributes->GetMaxMana());
	OnShieldChanged(RPGAttributes->GetShield(), RPGAttributes->GetMaxShield());
	GetCharacterMovement()->MaxWalkSpeed = RPGAttributes->GetMovementSpeed();
	OnStaminaChanged(RPGAttributes->GetStamina(), RPGAttributes->GetMaxStamina());
}

void ARPGSystemsCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	AddEquipmentToCharacterCapture(this);

	if (InventoryComponent)
	{
		InventoryComponent->InventoryList.OwningObject = this;
	}

	if (EquipmentComponent)
	{
		EquipmentComponent->EquipmentList.OwningObject = this;
		EquipmentComponent->BindInventoryDelegates(InventoryComponent);
	}

	if (CameraAimCurve)
	{
		FOnTimelineFloat  ProgressCallback;
		ProgressCallback.BindUFunction(this, FName("UpdateCameraAim"));
		CameraTimeline.AddInterpFloat(CameraAimCurve, ProgressCallback);
		CameraTimeline.SetLooping(false);
	}
}

void ARPGSystemsCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (CameraTimeline.IsPlaying() || CameraTimeline.IsReversing())
	{
		CameraTimeline.TickTimeline(DeltaSeconds);
	}
}

void ARPGSystemsCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGSystemsCharacter, InventoryComponent);
	DOREPLIFETIME(ARPGSystemsCharacter, EquipmentComponent);
}

void ARPGSystemsCharacter::AddEquipmentToCharacterCapture(AActor* Actor) const
{
	if (!IsValid(CharacterCaptureComponent)) return;
	CharacterCaptureComponent->ShowOnlyActors.Add(Actor);
}

void ARPGSystemsCharacter::RemoveEquipmentFromCharacterCapture(AActor* Actor) const
{
	if (!IsValid(CharacterCaptureComponent)) return;
	CharacterCaptureComponent->ShowOnlyActors.Remove(Actor);
}

void ARPGSystemsCharacter::OnRep_InventoryComponent()
{
	InventoryComponent->InventoryList.OwningObject = this;
}

void ARPGSystemsCharacter::OnRep_EquipmentComponent()
{
	EquipmentComponent->EquipmentList.OwningObject = this;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ARPGSystemsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ARPGSystemsCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ARPGSystemsCharacter::StopJumping);

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ARPGSystemsCharacter::Interact);
		
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARPGSystemsCharacter::Move);

		//Sprint
		//EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ARPGSystemsCharacter::Sprint);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARPGSystemsCharacter::Look);

		//Vault
		//EnhancedInputComponent->BindAction(VaultAction, ETriggerEvent::Started,this,&ARPGSystemsCharacter::TryVault);

		//Assasination
		//EnhancedInputComponent->BindAction(AssasinationAction, ETriggerEvent::Started,this,&ARPGSystemsCharacter::TryVault);
	}
	else
	{	
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

AEquipmentActor* ARPGSystemsCharacter::GetEquipmentActor(FGameplayTag AttachPoint)
{
	for (UEquipmentInstance* Instance : EquipmentInstances)
	{
		if (AEquipmentActor* EquipmentActor = Instance->GetActorAttached(AttachPoint))
		{
			return EquipmentActor;
		}
	}
	return nullptr;
}

void ARPGSystemsCharacter::SetEquipment(UEquipmentInstance* NewInstance)
{
	if (!IsValid(NewInstance))
	{
		return;
	}
	
	EquipmentInstances.Add(NewInstance);

	for (const TPair Pair : NewInstance->GetEquipmentActors())
	{
		AddEquipmentToCharacterCapture(Pair.Value);
	}
}
void ARPGSystemsCharacter::RemoveEquipment(UEquipmentInstance* InstanceToRemove)
{
	if (IsValid(InstanceToRemove))
	{
		EquipmentInstances.Remove(InstanceToRemove);
	}
}

void ARPGSystemsCharacter::ChangueEquipmentAttachPoint(FGameplayTag OldAttachTag,FGameplayTag NewAttachTag)
{
	UEquipmentInstance* CurrentInstance = nullptr;
	UEquipmentInstance* AlreadyExistedInstance = nullptr;
	
	for (UEquipmentInstance* Instance : EquipmentInstances)
	{
		if (Instance->HasAnActorAttached(NewAttachTag))
		{
			AlreadyExistedInstance = Instance;
		}
		if (Instance->HasAnActorAttached(OldAttachTag))
		{
			CurrentInstance = Instance;
		}
	}

	if (AlreadyExistedInstance != nullptr && AlreadyExistedInstance != CurrentInstance)
	{
		AlreadyExistedInstance->ChangeAttachPoint(NewAttachTag,OldAttachTag);
	}
	
	CurrentInstance->ChangeAttachPoint(OldAttachTag,NewAttachTag);
}


void ARPGSystemsCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ARPGSystemsCharacter::Jump()
{
	if (ThisFrameInteractActor || !IsValid(VaultComponent))
	{
		return;
	}
	
	VaultComponent->TryVault();
	
	if (!VaultComponent->GetCanVault())
	{
		Super::Jump();
	}
}

void ARPGSystemsCharacter::StopJumping()
{
	if (bPressedJump)
	{
		Super::StopJumping();
	}
}

void ARPGSystemsCharacter::Interact()
{
	if (ThisFrameInteractActor == nullptr) return;
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!IsValid(ASC)) return;
	
	TObjectPtr<APlayerController> PlayerController = ASC->AbilityActorInfo->PlayerController.Get();
	IInteractableInterface::Execute_BeginInteract(ThisFrameInteractActor.GetObject(),PlayerController);
}

// void ARPGSystemsCharacter::Sprint()
// {
// 	FTimerHandle SprintTimerHandle;
// 	GetWorld()->GetTimerManager().SetTimer(SprintTimerHandle, []()
// 	{
// 		//Decrease Stamina
// 		
// 	},0.1f,true);
// }

	void ARPGSystemsCharacter::Look(const FInputActionValue& Value)
	{
		// input is a Vector2D
		FVector2D LookAxisVector = Value.Get<FVector2D>();
		
		if (Controller != nullptr)
		{
			AddControllerYawInput(LookAxisVector.X);

			float CurrentPitch = FRotator::NormalizeAxis(GetControlRotation().Pitch);

			const float MinPitch = -40.f;
			const float MaxPitch =  60.f;
			
			APlayerController* PC = Cast<APlayerController>(Controller);
			float InputScale = PC ? PC->InputPitchScale_DEPRECATED : 1.f;
			float NewPitch = FMath::Clamp(CurrentPitch + (LookAxisVector.Y * InputScale), MinPitch, MaxPitch);

			FRotator NewRotation = GetControlRotation();
			NewRotation.Pitch = NewPitch;
			Controller->SetControlRotation(NewRotation);
		}
	}

void ARPGSystemsCharacter::Death_Implementation()
{
	//GetWorld()->GetFirstPlayerController()
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(PlayerController))
	{
		return;
	}
	
	DisableInput(PlayerController);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);
}
