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
#include "Character/Animation/RPGAnimInstance.h"
#include "Character/Components/RPGCharacterMovement.h"
#include "Data/CharacterClassInfo.h"
#include "Game/PlayerState/RPGPlayerState.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Libraries/RPGAbilitySystemLibrary.h"
#include "Character/Components/RPGMotionWarpingComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Equipment/EquipmentActor.h"
#include "Equipment/EquipmentManagerComponent.h"
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

	CharacterCaptureSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CharacterCaptureSpringArm"));
	CharacterCaptureSpringArm->SetupAttachment(GetCapsuleComponent());

	CharacterCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CharacterCaptureComponent"));
	CharacterCaptureComponent->SetupAttachment(CharacterCaptureSpringArm);

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetIsReplicated(true);
	
	EquipmentComponent = CreateDefaultSubobject<UEquipmentManagerComponent>(TEXT("EquipmentManagerComponent"));
	EquipmentComponent->SetIsReplicated(true);
	
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	VaultSphereRadiusFirstCheck = 5.f;
	VaultSphereRadiusSecondCheck = 10.f;
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

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARPGSystemsCharacter::Move);

		//Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ARPGSystemsCharacter::Sprint);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARPGSystemsCharacter::Look);

		//Vault
		EnhancedInputComponent->BindAction(VaultAction, ETriggerEvent::Started,this,&ARPGSystemsCharacter::TryVault);

		//Assasination
		EnhancedInputComponent->BindAction(AssasinationAction, ETriggerEvent::Started,this,&ARPGSystemsCharacter::TryVault);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ARPGSystemsCharacter::SetRightHandEquipment(AEquipmentActor* NewRightHandEquipment)
{
	RightHandEquipment = NewRightHandEquipment;
	AddEquipmentToCharacterCapture(NewRightHandEquipment);
}

void ARPGSystemsCharacter::SetLeftHandEquipment(AEquipmentActor* NewLeftHandEquipment)
{
	LeftHandEquipment = NewLeftHandEquipment;
	AddEquipmentToCharacterCapture(NewLeftHandEquipment);
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
	TryVault();
	if (!CanVault)
		Super::Jump();
}

void ARPGSystemsCharacter::StopJumping()
{
	if (bPressedJump)
		Super::StopJumping();
}

void ARPGSystemsCharacter::Sprint()
{
	FTimerHandle SprintTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(SprintTimerHandle, []()
	{
		//Decrease Stamina
		
	},0.1f,true);
}

void ARPGSystemsCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
//////////////////////////////////
/// Vaulting -> Move To Component
/// 
void ARPGSystemsCharacter::TryVault()
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	FHitResult OutHitFirstCheck;
	for (int i = 0; i < NumTracesFirstCheck;i++)
	{
		
		FVector StartLocation = GetActorLocation() + FVector(0, 0, i * VaultZOffsetFirstTrace);
		FVector EndLocation = StartLocation + GetActorForwardVector() * MaxDetectionDistance;
		
		bool bShereHit = UKismetSystemLibrary::SphereTraceSingle(
			this,
			StartLocation,
			EndLocation,
			VaultSphereRadiusFirstCheck,
			TraceTypeQuery1,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForDuration,
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
		FVector ForwardOffset = GetActorForwardVector() * (j * VaultZOffsetSecondTrace);
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,
			FString::Printf(TEXT("checks: %d"),j));
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
			EDrawDebugTrace::ForDuration,
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
			FVector DirectionWithOffset = GetActorForwardVector() * 80.f;
			FVector StartLocationThirdCheck = OutHit.TraceStart + DirectionWithOffset;
			FVector EndLocationThirdCheck = StartLocationThirdCheck - FVector(0,0,1000);
			bool bLineTraceHit = UKismetSystemLibrary::LineTraceSingle(this,StartLocationThirdCheck,EndLocationThirdCheck,
				TraceTypeQuery1,false,ActorsToIgnore,EDrawDebugTrace::ForDuration,OutHitThirdCheck,
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

void ARPGSystemsCharacter::VaultMotionWarp()
{
	USkeletalMeshComponent* MeshRef = GetMesh();
	if (!IsValid(MeshRef))
	{
		return;
	}
	
	float MeshZWorldLocation = MeshRef->GetComponentLocation().Z;
	float MinZOffset = MeshZWorldLocation - LandingZOffset;
	float MaxZOffset = MeshZWorldLocation + LandingZOffset;
	
	if (CanVault && VaultLandPos.Z >=  MinZOffset && VaultLandPos.Z <= MaxZOffset)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (!IsValid(AnimInstance) || !IsValid(VaultMontage))
		{
			return;
		}
		
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		SetActorEnableCollision(false);
		
		FMotionWarpingTarget TargetStart;
		TargetStart.Location = VaultStartPos;
		TargetStart.Name = FName("VaultStart");
		TargetStart.Rotation = GetActorRotation();
		MotionWarpingComponent->AddOrUpdateWarpTarget(TargetStart);
		FMotionWarpingTarget TargetMiddle;
		TargetMiddle.Location = VaultMiddlePos;
		TargetMiddle.Name = FName("VaultMiddle");
		TargetMiddle.Rotation = GetActorRotation();
		MotionWarpingComponent->AddOrUpdateWarpTarget(TargetMiddle);
		FMotionWarpingTarget TargetLand;
		TargetLand.Location = VaultLandPos;
		TargetLand.Name = FName("VaultLand");
		TargetLand.Rotation = GetActorRotation();
		MotionWarpingComponent->AddOrUpdateWarpTarget(TargetLand);

		AnimInstance->OnMontageEnded.AddDynamic(this, &ARPGSystemsCharacter::OnVaultCompleted);
		AnimInstance->Montage_Play(VaultMontage);
	}
}

void ARPGSystemsCharacter::OnVaultCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	SetActorEnableCollision(true);
	CanVault = false;
	VaultLandPos = FVector(0, 0, 20000);
	
	GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &ARPGSystemsCharacter::OnVaultCompleted);
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
