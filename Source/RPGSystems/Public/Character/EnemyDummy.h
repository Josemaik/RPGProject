// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "Interfaces/AssasinationInterface.h"
#include "EnemyDummy.generated.h"

class USphereComponent;

UCLASS()
class RPGSYSTEMS_API AEnemyDummy : public AEnemyBase, public IAssasinationInterface
{
	GENERATED_BODY()

public:
	AEnemyDummy(const FObjectInitializer& ObjectInitializer);

	/*Assasination Interface*/
	virtual void StealthBackAssasin(FVector& RefLocation, FRotator& AssasinRotation) override;

	UFUNCTION(blueprintNativeEvent)
	void SetWidgetVisibility(bool value);
protected:
	virtual void BeginPlay() override;
private:
	void RagDoll();
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Components", meta=(AllowPrivateAccess=true))
	TObjectPtr<USphereComponent> AssasinationRadius;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Components", meta=(AllowPrivateAccess=true))
	TObjectPtr<USkeletalMeshComponent> AssasinRef;
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Animation")
	UAnimMontage* AssasinationAnimMontage;
};
