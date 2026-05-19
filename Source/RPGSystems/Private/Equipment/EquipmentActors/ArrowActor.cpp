// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentActors/ArrowActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Libraries/RPGAbilitySystemLibrary.h"


// Sets default values
AArrowActor::AArrowActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// EquipmentMesh->SetupAttachment(nullptr);
	// SetRootComponent(EquipmentMesh);
	// EquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EquipmentMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	EquipmentMesh->SetCollisionObjectType(ECC_WorldDynamic);
	EquipmentMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	EquipmentMesh->SetIsReplicated(true);

	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	ProjectileMovement->SetActive(false);
	ProjectileMovement->bAutoActivate = false;

	ProjectileMovement->InitialSpeed      = 0.f;
	ProjectileMovement->MaxSpeed          = MaxSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce     = false;
	ProjectileMovement->ProjectileGravityScale = GravityScale;

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	OverlapSphere->SetupAttachment(EquipmentMesh);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	OverlapSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AArrowActor::Launch(FVector Direction, float ChargeRatio)
{
	if (bHasLaunched) return;
	bHasLaunched = true;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	
	//SetActorRotation(Direction.Rotation());
	
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	
	float Speed = MaxSpeed * FMath::Clamp(ChargeRatio, 0.1f, 1.0f);
	
	ProjectileMovement->Velocity = Direction.GetSafeNormal() * Speed;
	ProjectileMovement->MaxSpeed = MaxSpeed;
	ProjectileMovement->SetActive(true);
}

// Called when the game starts or when spawned
void AArrowActor::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(OverlapSphere) && HasAuthority())
	{
		OverlapSphere->OnComponentBeginOverlap.AddDynamic(this,&ThisClass::OnSphereBeginOverlap);
	}
}

void AArrowActor::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner())
	{
		return;
	}

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor))
	{
		GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,"ArrowActor::OnSphereBeginOverlap");
		DamageEffectInfo.TargetASC = TargetASC;
		URPGAbilitySystemLibrary::ApplyDamageEffect(DamageEffectInfo);
		
		//Destroy();
		if (OtherComp)
		{
			FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, true);
			AttachToComponent(OtherComp, Rules, NAME_None);
		}
	}
}

