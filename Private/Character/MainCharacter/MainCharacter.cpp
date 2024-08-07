// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MainCharacter/MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Actors/Gun.h"
#include "Components/CapsuleComponent.h"
#include "GameMode/ShooterGameModeBase.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"


AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}


void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}

	Health = MaxHealth;
	CurrentAmmo = Ammo;

	Gun = GetWorld()->SpawnActor<AGun>(GunClass, GetActorLocation(), GetActorRotation());
	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
	if (Gun)
	{
		FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
		Gun->AttachToComponent(GetMesh(), TransformRules, FName("GunSocket"));
		Gun->SetOwner(this);
	}
}


void AMainCharacter::ReloadEnd()
{
	CharacterState = ECharacterState::NoState;
}

bool AMainCharacter::IsDead() const
{
	if (Health <= 0)
	{
		return true;
	}
	return false;
}

float AMainCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

int32 AMainCharacter::GetAmmo() const
{
	return CurrentAmmo;
}

int32 AMainCharacter::GetTotalAmmo() const
{
	return TotalAmmo;
}

void AMainCharacter::Move(const FInputActionValue& Value)
{
	if (CharacterState == ECharacterState::Reloading) return;

	FVector2D MoveAxis = Value.Get<FVector2D>();
	if (MoveAction)
	{
		FVector ForwardVector = GetActorForwardVector();
		AddMovementInput(ForwardVector, MoveAxis.X);

		FVector RightVector = GetActorRightVector();
		AddMovementInput(RightVector, MoveAxis.Y);
	}
}

void AMainCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxis = Value.Get<FVector2D>();
	if (LookAction)
	{
		AddControllerYawInput(LookAxis.X);
		AddControllerPitchInput(LookAxis.Y);
	}
}

void AMainCharacter::Jump()
{
	if (JumpAction)
	{
		Super::Jump();
	}
}

void AMainCharacter::Run(const FInputActionValue& Value)
{
	bool isRunning = Value.Get<bool>();
	if (RunAction && isRunning)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 150.f;
	}
}

void AMainCharacter::Reload()
{
	if (TotalAmmo <= 0 || CurrentAmmo == Ammo) return;

	CharacterState = ECharacterState::Reloading;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, Gun->GetActorLocation());
		AnimInstance->Montage_Play(ReloadMontage);
	}


	if (TotalAmmo < RemainingAmmoSpace)
	{
		CurrentAmmo = CurrentAmmo + TotalAmmo;
		TotalAmmo = 0;
	}
	else
	{
		TotalAmmo = TotalAmmo - RemainingAmmoSpace;
		CurrentAmmo = CurrentAmmo + RemainingAmmoSpace;
	}
}

void AMainCharacter::Win()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(WinMontage);
	}
}

void AMainCharacter::PullTrigger()
{
	if (CharacterState == ECharacterState::Reloading) return;

	if (PullTriggerAction && Gun)
	{
		if (CurrentAmmo <= 0)
		{
			UGameplayStatics::PlaySoundAtLocation(this, EmptyTriggerSound, Gun->GetActorLocation());
			return;
		}
		else if (CurrentAmmo > 0)
		{
			--CurrentAmmo;
			RemainingAmmoSpace = Ammo - CurrentAmmo;
			Gun->Shoot();
		}
	}
}


void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMainCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMainCharacter::Jump);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AMainCharacter::Run);
		EnhancedInputComponent->BindAction(PullTriggerAction, ETriggerEvent::Started, this, &AMainCharacter::PullTrigger);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AMainCharacter::Reload);
	}
}

float AMainCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsDead())
	{
		AShooterGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AShooterGameModeBase>();
		if (GameMode)
		{
			GameMode->PawnKilled(this);
		}
		DetachFromControllerPendingDestroy();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		return -1;
	}

	float DamageToApply = Super::TakeDamage(DamageAmount,DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Min(Health, DamageToApply);
	Health -= DamageToApply;
	UE_LOG(LogTemp, Warning, TEXT("Current Health: %f"), Health);

	return DamageToApply;
}


