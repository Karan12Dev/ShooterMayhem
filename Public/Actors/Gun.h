// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

UCLASS()
class SHOOTER_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	AGun();
	virtual void Tick(float DeltaTime) override;

	void Shoot();

		
protected:
	virtual void BeginPlay() override;


private:
	bool GunTrace(FHitResult& HitResult, FVector& ShotDirection);
	AController* GetOwnerController() const;


	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* GunMesh;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	USoundBase* MuzzleSound;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	UParticleSystem* ImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	USoundBase* ImpactSound;

	UPROPERTY(EditDefaultsOnly)
	float MaxRange = 6000.f;

	UPROPERTY(EditAnywhere)
	float Damage = 15.f;

};
