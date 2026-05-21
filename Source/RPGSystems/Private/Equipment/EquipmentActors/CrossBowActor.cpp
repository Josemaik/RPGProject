// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentActors/CrossBowActor.h"

#include "AbilitySystem/NativeTags/RPGInventoryTags.h"
#include "Character/RPGSystemsCharacter.h"
#include "Components/ArrowComponent.h"
#include "Equipment/EquipmentActors/ArrowActor.h"


// Sets default values
ACrossBowActor::ACrossBowActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(GetRootComponent());
	
	MuzzleDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("MuzzleDirection"));
	MuzzleDirection->SetupAttachment(GetRootComponent());
}

void ACrossBowActor::Shoot()
{
	if (!IsValid(LastSpawnedArrow))
	{
		return;
	}
	
	APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
	if (!IsValid(PC)) return;

	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector TraceStart = CameraLocation;
	FVector TraceEnd = CameraLocation  + CameraRotation.Vector() * 50000.f;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);
	Params.AddIgnoredActor(LastSpawnedArrow);

	FVector TargetPoint;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params))
	{
		TargetPoint = HitResult.ImpactPoint;
	}
	else
	{
		TargetPoint = TraceEnd;
	}

	FVector Direction = (TargetPoint - LastSpawnedArrow->GetActorLocation()).GetSafeNormal();

	float Distance = Direction.Size();

	float CompensationScale = 0.2f;
	Direction.Z += Distance * CompensationScale;

	Direction = Direction.GetSafeNormal();
	
	LastSpawnedArrow->Launch(Direction,1.f);
	LastSpawnedArrow = nullptr;

	bHasArrow = false;
}

void ACrossBowActor::SpawnArrow(FDamageEffectInfo DamageEffectInfo)
{
	if (!IsValid(OwnerCharacter) || !IsValid(ArrowComponent) || !IsValid(MuzzleDirection))
	{
		return;
	}

	const FVector SpawnLocation = ArrowComponent->GetComponentLocation();
	const FRotator SpawnRotation = ArrowComponent->GetComponentRotation();

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(SpawnRotation.Quaternion());

	TSoftClassPtr<AEquipmentActor> SoftClass = OwnerCharacter->GetEquipmentClassBySlotTag(RPGInventoryTags::EquipmentSlot::Bolts);

	if (SoftClass.IsNull()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnArrow: SoftClass es null, no entry for Bolts"));
		return;
	}

	TSubclassOf<AArrowActor> ArrowClass = SoftClass.LoadSynchronous();

	if (!ArrowClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnArrow: ArrowClass synchornous loading failed"));
		return;
	}
	
	LastSpawnedArrow = Cast<AArrowActor>(GetWorld()->SpawnActorDeferred<AArrowActor>(ArrowClass,
			SpawnTransform, this));
	if (IsValid(LastSpawnedArrow))
	{
		LastSpawnedArrow->DamageEffectInfo = DamageEffectInfo;
		
		LastSpawnedArrow->FinishSpawning(SpawnTransform);
		LastSpawnedArrow->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("arrow_socket_hand_r")));
		
		bHasArrow = true;
	}
}

void ACrossBowActor::AttachArrow() const
{
	if (!IsValid(LastSpawnedArrow) || !IsValid(ArrowComponent))
	{
		return;
	}
	LastSpawnedArrow->GetRootComponent()->DetachFromComponent(
		FDetachmentTransformRules::KeepWorldTransform);
	//LastSpawnedArrow->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	LastSpawnedArrow->AttachToComponent(
			ArrowComponent,
			  FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

// Called when the game starts or when spawned
void ACrossBowActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACrossBowActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

