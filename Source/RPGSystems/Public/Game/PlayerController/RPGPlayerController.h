// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RPGPlayerController.generated.h"

class UInventoryComponent;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API ARPGPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ARPGPlayerController();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated)
	TObjectPtr<UInventoryComponent> InventoryComponent;
};
