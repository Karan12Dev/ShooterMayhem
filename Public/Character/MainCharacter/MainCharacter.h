// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MainCharacter.generated.h"

class UInputAction;

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	NoState,
	Reloading
};

UCLASS()
class SHOOTER_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMainCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void PullTrigger();
	void Reload();
	void Win();

	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	UFUNCTION(BlueprintPure)
	int32 GetAmmo() const;

	UFUNCTION(BlueprintPure)
	int32 GetTotalAmmo() const;


protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ReloadEnd();

private:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* MappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;
	void Move(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookAction;
	void Look(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* JumpAction;
	virtual void Jump() override;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* RunAction;
	void Run(const FInputActionValue& Value);
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	float RunSpeed;

	UPROPERTY(EditAnywhere, Category = "Class")
	TSubclassOf<class AGun> GunClass;

	UPROPERTY()
	AGun* Gun;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* PullTriggerAction;

	UPROPERTY(EditAnywhere)
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere)
	float Health;

	UPROPERTY(VisibleInstanceOnly, Category = "Combat")
	ECharacterState CharacterState = ECharacterState::NoState;

	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 Ammo = 8;

	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 CurrentAmmo;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 TotalAmmo = 40;

	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 RemainingAmmoSpace;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ReloadAction;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* ReloadMontage;

	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	USoundBase* EmptyTriggerSound;

	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	USoundBase* ReloadSound;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* WinMontage;

};
