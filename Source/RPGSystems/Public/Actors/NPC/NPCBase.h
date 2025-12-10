// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "NPCBase.generated.h"

UCLASS()
class RPGSYSTEMS_API ANPCBase : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	
	ANPCBase();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Custom Values|Name")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Values|Options")
	TArray<FGameplayTag> Options;
private:
	
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Mesh;
};
