// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AssasinationInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UAssasinationInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGSYSTEMS_API IAssasinationInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void StealthBackAssasin(FVector& RefLocation,FRotator& AssasinRotation) = 0;
};
