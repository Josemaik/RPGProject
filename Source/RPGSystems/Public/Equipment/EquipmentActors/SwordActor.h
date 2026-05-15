// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentActor.h"
#include "SwordActor.generated.h"

class UArrowComponent;

UCLASS()
class RPGSYSTEMS_API ASwordActor : public AEquipmentActor
{
	GENERATED_BODY()

public:
	
	ASwordActor();

	TObjectPtr<UArrowComponent> GetSwordTopPoint() { return SwordTopPoint; }
	TObjectPtr<UArrowComponent> GetSwordButtomPoint() { return SwordButtomPoint; }
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	TObjectPtr<UArrowComponent> SwordTopPoint;

	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	TObjectPtr<UArrowComponent> SwordButtomPoint;
};
