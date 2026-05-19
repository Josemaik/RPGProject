// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentActor.h"
#include "AbilitySystem/RPGAbilityTypes.h"
#include "ArrowActor.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class RPGSYSTEMS_API AArrowActor : public AEquipmentActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AArrowActor();
	
	UFUNCTION(BlueprintCallable, Category = "Arrow")
	void Launch(FVector Direction, float ChargeRatio = 1.0f);

	UPROPERTY(BlueprintReadWrite)
	FDamageEffectInfo DamageEffectInfo;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arrow")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	
	UPROPERTY(EditDefaultsOnly, Category = "Arrow")
	float MaxSpeed = 6000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arrow")
	float GravityScale = 1.5f;
	
private:
	bool bHasLaunched = false;
	
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arrow",meta=(AllowPrivateAccess="true"))
	TObjectPtr<USphereComponent> OverlapSphere;
};
