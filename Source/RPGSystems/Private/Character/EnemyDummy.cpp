// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/EnemyDummy.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"


AEnemyDummy::AEnemyDummy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	AssasinationRadius = CreateDefaultSubobject<USphereComponent>(TEXT("AssasinationSphereRadius"));
	AssasinationRadius->SetupAttachment(RootComponent);
	AssasinationRadius->SetSphereRadius(65.f);
	AssasinationRadius->SetRelativeLocation(FVector(-30.f,0.f,0.f));
	AssasinationRadius->Activate(true);
	AssasinationRadius->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);

	AssasinRef = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("AssasinRef"));
	AssasinRef->SetupAttachment(RootComponent);
	AssasinRef->SetHiddenInGame(true);

	GetMesh()->SetCollisionResponseToAllChannels(ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn,ECR_Ignore);
}

void AEnemyDummy::StealthBackAssasin(FVector& RefLocation, FRotator& AssasinRotation)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!IsValid(AnimInstance))
	{
		return;
	}
	
	AnimInstance->Montage_Play(AssasinationAnimMontage);

	RagDoll();
	SetWidgetVisibility(false);

	RefLocation = AssasinRef->GetComponentLocation();
	AssasinRotation = AssasinRef->GetComponentRotation();
}

void AEnemyDummy::SetWidgetVisibility_Implementation(bool value)
{
}

void AEnemyDummy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyDummy::RagDoll()
{
	FTimerHandle RagDollTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(RagDollTimerHandle, [this]()
	{
		GetMesh()->SetSimulatePhysics(true);
		AssasinationRadius->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	},2.5f,false);
}

