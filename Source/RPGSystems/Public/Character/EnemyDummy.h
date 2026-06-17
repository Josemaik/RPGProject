// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "Interfaces/AssasinationInterface.h"
#include "Interfaces/EnemyAIInterface.h"
#include "EnemyDummy.generated.h"

class USphereComponent;

UCLASS()
class RPGSYSTEMS_API AEnemyDummy : public AEnemyBase, public IAssasinationInterface, public IEnemyAIInterface
{
	GENERATED_BODY()

public:
	AEnemyDummy(const FObjectInitializer& ObjectInitializer);

	/*Assasination Interface*/
	virtual void StealthBackAssasin(FVector& RefLocation, FRotator& AssasinRotation) override;

	UFUNCTION(blueprintNativeEvent)
	void SetWidgetVisibility(bool value);

	virtual float Attack() override;
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

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Animation")
	UAnimMontage* AttackMontage;
};
