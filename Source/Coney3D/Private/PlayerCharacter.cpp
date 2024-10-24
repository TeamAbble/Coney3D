// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BaseWeapon.h"
#include "Net/UnrealNetwork.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	if (GetLocalRole() == ROLE_Authority && weaponBlueprint) {

		FActorSpawnParameters spawnParams;
		spawnParams.Owner = this;
		spawnParams.Instigator = GetInstigator();

		ABaseWeapon* weapon = GetWorld()->SpawnActor<ABaseWeapon>(weaponBlueprint, spawnParams);
		weapon->AttachToComponent(weaponPointRef, FAttachmentTransformRules::KeepRelativeTransform);
		weapon->SetActorRelativeTransform(FTransform());

		weapon->connectedPlayer = this;
	}
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageDealer)
{
	Health -= DamageAmount;
	//if (Health <= 0) {
	//Do whatever we need to do when the player dies
	//}
	
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(DamageAmount));
	}
	return DamageAmount;
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Jumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &APlayerCharacter::TryDash);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacter::SetSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::SetSprint);
		//Registers the fire callback for both started and completed
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &APlayerCharacter::SetFire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &APlayerCharacter::SetFire);
	}

}
void APlayerCharacter::SetFire(const FInputActionValue& value) {
	fireInput = value.Get<bool>();
}
void APlayerCharacter::SetSprint(const FInputActionValue& value) {
	sprinting = value.Get<bool>();
	Sprint(sprinting);
}
void APlayerCharacter::Move(const FInputActionValue& value)
{
	MovementVector = value.Get<FVector2D>();

	if (Controller != nullptr) {
		Rotation = Controller->GetControlRotation();
		YawRotation = FRotator(0, Rotation.Yaw, 0);

		UpdateDirection();

		AddMovementInput(ForwardDir, MovementVector.Y);
		AddMovementInput(RightDir, MovementVector.X);


	}
}

void APlayerCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookAxisVector = value.Get<FVector2D>();
	if (Controller != nullptr) {
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
	if (aimRef) {
		aimRef->SetWorldRotation(GetControlRotation());
	}
}

void APlayerCharacter::Jumping()
{
	Jump();
}

void APlayerCharacter::Sprint_Implementation(bool sprint)
{
	sprinting = sprint;
	GetCharacterMovement()->MaxWalkSpeed = sprinting ? SprintSpeed : WalkSpeed;
}

void APlayerCharacter::UpdateDirection()
{
	ForwardDir = GetActorForwardVector();
	RightDir = GetActorRightVector();
}

void APlayerCharacter::Dash_Implementation(FVector inputVector)
{
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(MovementVector.X));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(MovementVector.Y));

	}
	FVector Up = FVector(0, 0, DashUpwardVelocity);
	if (inputVector != FVector(0, 0, 0)) {
		LaunchCharacter(FVector(ForwardDir * inputVector.Y + RightDir * inputVector.X) * DashSpeed + Up, true, true);
	}
	else {
		UpdateDirection();
		LaunchCharacter(ForwardDir * DashSpeed + Up, true, true);
	}
	
}

void APlayerCharacter::TryDash()
{
	UpdateDirection();
	Dash(ForwardDir * MovementVector.Y + RightDir * MovementVector.X);
}

bool APlayerCharacter::GetFireInput()
{
	return fireInput;
}


FVector2D APlayerCharacter::GetMovementVector()
{
	return FVector2D(MovementVector);
}

void APlayerCharacter::Vault()
{
	FHitResult Hit;
	FVector StartPos{ GetActorLocation() };
	FVector EndPos{ StartPos + GetActorForwardVector() * 80 };
	FCollisionObjectQueryParams ObjectsToCheck;
	ObjectsToCheck.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	FCollisionQueryParams ObjectsToIgnore;
	ObjectsToIgnore.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByObjectType(Hit, StartPos, EndPos, ObjectsToCheck, ObjectsToIgnore);
	DrawDebugLine(GetWorld(), StartPos, EndPos, FColor::Red, true);

	if (Hit.Component == nullptr) {
		bShouldClimb = false;
		return;
	}

	FVector FrontHitLocation = Hit.Location;

	FVector WallPos{ Hit.Location };
	FVector WallNormal{ Hit.Normal };
	FRotator WallNormalRotationX = UKismetMathLibrary::MakeRotFromX(WallNormal);
	FVector WallNormalForwardVector = UKismetMathLibrary::GetForwardVector(WallNormalRotationX);
	StartPos = (WallNormalForwardVector * -5) + WallPos + FVector{ 0, 0, 200 };
	EndPos = StartPos - FVector{ 0, 0, 200 };
	GetWorld()->LineTraceSingleByObjectType(Hit, StartPos, EndPos, ObjectsToCheck, ObjectsToIgnore);
	DrawDebugLine(GetWorld(), StartPos, EndPos, FColor::Green, true);
	FVector TopHitPos{ Hit.Location };
	float WallHeight{ 0 };
	if (Hit.Component != nullptr) {
		WallHeight = TopHitPos.Z - FrontHitLocation.Z;
	}
	StartPos = (WallNormalForwardVector * -50) + WallPos + FVector{ 0,0,200 };
	EndPos = StartPos - FVector{ 0,0,200 };
	GetWorld()->LineTraceSingleByObjectType(Hit, StartPos, EndPos, ObjectsToCheck, ObjectsToIgnore);
	DrawDebugLine(GetWorld(), StartPos, EndPos, FColor::Blue, true);

	if (Hit.Component != nullptr) {
		VaultPos = EndPos;
	}
	else {
		VaultPos = { 0,0,0 };
	}
}