// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerCharacter.generated.h"

class ABaseWeapon;

UCLASS()
class CONEY3D_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	FVector ForwardDir;
	FVector RightDir;
	FVector2D MovementVector;
	FRotator Rotation;
	int points = 0;
	FRotator YawRotation;
	FTimerHandle DashTimer;
	FTimerHandle RespawnTimer;
	bool CanDash = true;

	
	UPROPERTY(EditAnywhere, Category = "Health And Damage") 
	float MaxHealth = 250.0f;
	float Health = MaxHealth;
//<summary>
//Applies Damage to the player
//If health is below or equal to zero player dies
//Point is granted to other player on death
//</summary>
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageDealer)override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	static bool GamePaused;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") class UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") class UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") class UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") class UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") class UInputAction* DashAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") class UInputAction* FireAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health and Damage") FVector SpawnLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed") float DashSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed") float DashUpwardVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed") float WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed") float SprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed") float DashCooldown=2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health and Damage")float RespawnTime = 1;
	

	float currentTime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") bool fireInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons") USceneComponent* weaponPointRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons") USceneComponent* aimRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup") UCameraComponent* playerCam;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons") TSubclassOf<class ABaseWeapon> weaponBlueprint;
	bool sprinting;
	bool bShouldClimb;
	FVector VaultPos;
	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void SetFire(const FInputActionValue& value);
	UFUNCTION()
	void SetSprint(const FInputActionValue& value);
	UFUNCTION()
	void Jumping();
	void Sprint(bool sprint);
	void UpdateDirection();
	void Dash(FVector forward, FVector right);
	void TryDash();
	void ResetDash();
	bool GetFireInput() const;
	void Die(AActor *OtherPlayer);
	void Respawn();
	void GainPoint();
	FVector2D GetMovementVector() const;
	UFUNCTION()
	void Vault();
	bool Dead = false;
	ABaseWeapon* weapon;
	UFUNCTION(BlueprintCallable)
	float GetCurrentHealth() const;
	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const;
};
