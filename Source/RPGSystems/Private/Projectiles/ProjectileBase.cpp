// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/ProjectileBase.h"

#include "AbilitySystem/RPGAbilityTypes.h"
#include "GameFramework/ProjectileMovementComponent.h"


AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	
	ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMeshComponent");
	SetRootComponent(ProjectileMeshComponent);
	ProjectileMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProjectileMeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
	ProjectileMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ProjectileMeshComponent->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	ProjectileMeshComponent->SetIsReplicated(true);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
}

void AProjectileBase::SetProjectileParams(const FProjectileParams& Params)
{
	if (!IsValid(ProjectileMeshComponent) || !IsValid(ProjectileMovementComponent))
	{
		return;
	}

	ProjectileMeshComponent->SetStaticMesh(Params.ProjectileMesh);
	
	ProjectileMovementComponent->InitialSpeed = Params.InitialSpeed;
	ProjectileMovementComponent->ProjectileGravityScale = Params.GravityScale;
	ProjectileMovementComponent->bShouldBounce = Params.bShouldBounce;
	ProjectileMovementComponent->Bounciness = Params.Bounciness;
}


