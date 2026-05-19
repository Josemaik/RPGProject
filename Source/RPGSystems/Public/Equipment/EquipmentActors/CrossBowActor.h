// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentActor.h"
#include "CrossBowActor.generated.h"

class UArrowComponent;

UCLASS()
class RPGSYSTEMS_API ACrossBowActor : public AEquipmentActor
{
	GENERATED_BODY()

	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Equipment")
	TObjectPtr<UArrowComponent> ArrowComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")  
	TObjectPtr<UArrowComponent> MuzzleDirection; 
	// Sets default values for this actor's properties
	ACrossBowActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
