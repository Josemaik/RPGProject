// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterBase.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACharacterBase::InitAbilityActorInfo()
{
	//Intentionally left blank for now
}

void ACharacterBase::BindCallbacksToDependencies()
{
	//Intentionally left blank for now
}

void ACharacterBase::InitClassDefaults()
{
	//Intentionally left blank for now
}

