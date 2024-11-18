// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimSequence.h"

#include "PlayerCharacter.generated.h"


class ABaseWeapon;
class UInputAction;


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
	virtual void PossessedBy(AController* NewController) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(BlueprintReadWrite) bool GamePaused;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") class UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* DashAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* FireAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* WeaponCycleAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* AimAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* ViewSwitchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")UInputAction* ReloadAction;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons") TSubclassOf<ABaseWeapon> weaponBlueprint;
	bool sprinting;
	bool bShouldClimb;
	FVector VaultPos;
	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void TryAim(const FInputActionValue& value);
	void SwitchView(const FInputActionValue& value);
	UFUNCTION()
	void SetFire(const FInputActionValue& value);
	UFUNCTION()
	void CycleWeapons(const FInputActionValue& value);

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
	UFUNCTION(BlueprintCallable) ABaseWeapon* GetCurrentWeapon() const;
	ABaseWeapon* weapon;
	UFUNCTION(BlueprintCallable)
	float GetCurrentHealth() const;
	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const;

	UPROPERTY(BlueprintReadOnly) TArray<ABaseWeapon*> weapons;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<TSubclassOf<ABaseWeapon>> weaponBlueprints;

	int8 weaponIndex;

	UFUNCTION(BlueprintCallable) int GetScore() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UCameraComponent* thirdPersonCamera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UCameraComponent* firstPersonCamera;
	bool firstPerson;
	bool aimed;
	float aimLerpProgress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float startFOV;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation") UAnimSequence* idleAnim;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation") UAnimMontage* equipAnim;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation") UAnimMontage* fireAnim;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation") UAnimMontage* reloadAnim;


	UFUNCTION(BlueprintImplementableEvent) void UpdateAnimations();
	UFUNCTION(BlueprintImplementableEvent) void ReloadEvent();
	UFUNCTION(BlueprintImplementableEvent) void FireEvent();


	UPROPERTY(EditAnywhere, BlueprintReadWrite) UMeshComponent* thirdPersonMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UMeshComponent* firstPersonMesh;
	
};

