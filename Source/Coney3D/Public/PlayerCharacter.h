// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacter.generated.h"

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
	FRotator YawRotation;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") class UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") class UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") class UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") class UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") class UInputAction* DashAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") class UInputAction* FireAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed") float DashSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed") float DashUpwardVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed") float WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed") float SprintSpeed;
	

	float currentTime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") bool fireInput;
	
	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void SetFire(const FInputActionValue& value);
	void Jumping();
	void Sprint();
	void UpdateDirection();
	void Dash();
	bool GetFireInput();
	FVector2D GetMovementVector();
};
