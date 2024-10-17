// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BaseWeapon.h"

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
	if (weaponBlueprint) {

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
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &APlayerCharacter::Dash);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::Sprint);
		//Registers the fire callback for both started and completed
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &APlayerCharacter::SetFire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &APlayerCharacter::SetFire);
	}

}
void APlayerCharacter::SetFire(const FInputActionValue& value) {
	fireInput = value.Get<bool>();
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

void APlayerCharacter::Sprint()
{
	if (GetCharacterMovement()->MaxWalkSpeed == WalkSpeed) {
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else {
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	}
	
}

void APlayerCharacter::UpdateDirection()
{
	ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
}

void APlayerCharacter::Dash()
{
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(MovementVector.X));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(MovementVector.Y));

	}
	FVector Up = FVector(0, 0, DashUpwardVelocity);
	if (MovementVector != FVector2D(0, 0)) {
		LaunchCharacter(FVector(ForwardDir * MovementVector.Y + RightDir * MovementVector.X) * DashSpeed + Up, true, true);
	}
	else {
		UpdateDirection();
		LaunchCharacter(ForwardDir * DashSpeed + Up, true, true);
	}
	
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


