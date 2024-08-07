// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Gun.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"


AGun::AGun()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	GunMesh->SetupAttachment(GetRootComponent());
}


void AGun::BeginPlay()
{
	Super::BeginPlay();
	
}

bool AGun::GunTrace(FHitResult& HitResult, FVector& ShotDirection)
{
	AController* OwnerController = GetOwnerController();
	if (OwnerController == nullptr) return false;

	FVector Location;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location, Rotation);
	FVector End = Location + Rotation.Vector() * MaxRange;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	ShotDirection = -Rotation.Vector();
	return GetWorld()->LineTraceSingleByChannel(HitResult, Location, End, ECollisionChannel::ECC_GameTraceChannel1, Params);
}

AController* AGun::GetOwnerController() const
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return nullptr;

	AController* OwnerController = OwnerPawn->GetController();
	return OwnerController;
}


void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AGun::Shoot()
{
	if (MuzzleEffect && MuzzleSound)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, GunMesh, FName("MuzzleFlashSocket"));
		UGameplayStatics::SpawnSoundAttached(MuzzleSound, GunMesh, FName("MuzzleFlashSocket"));
	}

	FHitResult HitResult;
	FVector ShotDirection;
	bool bSuccess = GunTrace(HitResult, ShotDirection);

	if (bSuccess)
	{
		if (ImpactEffect && ImpactSound)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, HitResult.Location, ShotDirection.Rotation());
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, HitResult.Location);
		}
		if (HitResult.GetActor())
		{
			FPointDamageEvent DamageEvent(Damage, HitResult, ShotDirection, nullptr);
			AController* OwnerController = GetOwnerController();
			HitResult.GetActor()->TakeDamage(Damage, DamageEvent, OwnerController, this);
		}
	}
}

