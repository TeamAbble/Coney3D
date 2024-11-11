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
	SpawnLocation = GetActorLocation();
	if (GetCharacterMovement()) {
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
	else {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Could not get CharacterMovement component!");
		}
	}

	if (weaponBlueprint) {
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = this;
		spawnParams.Instigator = GetInstigator();
		if (weaponBlueprints.Num() > 0) {
			for (size_t i = 0; i < weaponBlueprints.Num(); i++)
			{
				if (!weaponBlueprints[i])
					continue;

				ABaseWeapon* _weapon = GetWorld()->SpawnActor<ABaseWeapon>(weaponBlueprints[i], spawnParams);
				_weapon->AttachToComponent(weaponPointRef, FAttachmentTransformRules::KeepRelativeTransform);
				_weapon->SetActorRelativeTransform(FTransform());

				_weapon->connectedPlayer = this;
				_weapon->Hide();
				weapons.Add(_weapon);
			}
		}
		if (weapons[0]) {
			weapon = weapons[0];
			weapon->Show();
		}
	}
}
/// <summary>
/// <para>Deals damage to this player subtracting from Health</para>
/// <para>Checks if health is <=0, will call the Die() method if true</para>
/// </summary>
/// <param name="DamageAmount"></param>
/// <param name="DamageEvent"></param>
/// <param name="EventInstigator"></param>
/// <param name="DamageDealer"></param>
/// <returns></returns>
float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageDealer)
{
	if (Health>0) {
		Health -= DamageAmount;
		if (Health <= 0) {
			Die(DamageDealer);
		}
	}
	return DamageAmount;
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
		PlayerController->PlayerCameraManager->ViewPitchMax = 80.f;
		PlayerController->PlayerCameraManager->ViewPitchMin = -80.f;
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (weapon) {
		weapon->fireInput = GetFireInput();

		if (firstPersonCamera && thirdPersonCamera) {
			aimLerpProgress += (aimed ? DeltaTime : -DeltaTime) * weapon->aimSpeed;
			aimLerpProgress = FMath::Clamp(aimLerpProgress, 0, 1);
			if (firstPerson) {
				firstPersonCamera->FieldOfView = FMath::Lerp(startFOV, weapon->aimFOV, aimLerpProgress);
			}
			else {
				thirdPersonCamera->FieldOfView = FMath::Lerp(startFOV, weapon->aimFOV, aimLerpProgress);
			}
		}
	}
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
		//Registers the fire callback for both started and completed
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &APlayerCharacter::SetFire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &APlayerCharacter::SetFire);
		EnhancedInputComponent->BindAction(WeaponCycleAction, ETriggerEvent::Triggered, this, &APlayerCharacter::CycleWeapons);

		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &APlayerCharacter::TryAim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &APlayerCharacter::TryAim);

		EnhancedInputComponent->BindAction(ViewSwitchAction, ETriggerEvent::Triggered, this, &APlayerCharacter::SwitchView);
	}

}
void APlayerCharacter::TryAim(const FInputActionValue& value) {
	if (!firstPersonCamera || !thirdPersonCamera)
		return;
	aimed = value.Get<bool>();
}
void APlayerCharacter::SwitchView(const FInputActionValue& value)
{
	if (!firstPersonCamera || !thirdPersonCamera) {
		return;
	}

	firstPerson = !firstPerson;
	if (firstPerson) {
		firstPersonCamera->Activate();
		thirdPersonCamera->Deactivate();
	}
	else {
		firstPersonCamera->Deactivate();
		thirdPersonCamera->Activate();
	}
}
void APlayerCharacter::SetFire(const FInputActionValue& value) {
	fireInput = value.Get<bool>();
}
void APlayerCharacter::CycleWeapons(const FInputActionValue& value)
{
	float val = value.Get<float>();
	int newVal = FMath::RoundToInt(val);
	weaponIndex = (weaponIndex + newVal) % weapons.Num();
	if (weaponIndex < 0)
		weaponIndex = weapons.Num() - 1;
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Cycling weapon - In value: " + FString::SanitizeFloat(val) + " Cycle Value: " + FString::FromInt(newVal)));
	}
	for (size_t i = 0; i < weapons.Num(); i++)
	{
		if (!weapons[i])
			continue;

		weapons[i]->fireInput = false;
		if (i == weaponIndex) {
			weapons[i]->Show();
			weapon = weapons[i];
		}
		else {
			weapons[i]->Hide();
		}
	}
}
void APlayerCharacter::SetSprint(const FInputActionValue& value) {
	Sprint(!sprinting);

}
/// <summary>
/// <para>Gets the movement vector from the Move Input Action and adds the value to the player's built in movement input</para>
/// <para>Updates the direction to be used for dashing</para> 
/// </summary>
/// <param name="value"></param>
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
/// <summary>
/// Allows aiming with the mouse, so long as there is a controller present on this actor
/// </summary>
/// <param name="value"></param>
void APlayerCharacter::Look(const FInputActionValue& value)
{
	if (GamePaused)
		return;
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
	if(!GamePaused)
		Jump();
}

void APlayerCharacter::Sprint(bool sprint)
{
	sprinting = sprint;
	GetCharacterMovement()->MaxWalkSpeed = sprinting ? SprintSpeed : WalkSpeed;
}

void APlayerCharacter::UpdateDirection()
{
	ForwardDir = GetActorForwardVector();
	RightDir = GetActorRightVector();
}

void APlayerCharacter::Dash(FVector forward, FVector right)
{
	if (!CanDash)
		return;

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::SanitizeFloat(MovementVector.X));
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::SanitizeFloat(MovementVector.Y));

	}
	FVector Up = FVector(0, 0, DashUpwardVelocity);
	if (forward != FVector::Zero() || right != FVector::Zero()) {
		FVector vec = (forward + right);
		vec.Normalize();
		LaunchCharacter(vec * DashSpeed + Up, true, true);
	}
	else {
		UpdateDirection();
		LaunchCharacter(ForwardDir * DashSpeed + Up, true, true);
	}
	CanDash = false;
	GetWorld()->GetTimerManager().SetTimer(DashTimer, this, &APlayerCharacter::ResetDash, DashCooldown, false);
	
}
//Updates direction, then performs the dash
void APlayerCharacter::TryDash()
{
	if (!GamePaused) {
		UpdateDirection();
		Dash(ForwardDir * MovementVector.Y, RightDir * MovementVector.X);
	}
}
//Sets CanDash to true
void APlayerCharacter::ResetDash()
{
	CanDash = true;
}

bool APlayerCharacter::GetFireInput() const
{
	return fireInput && !Dead && !GamePaused;
}
/// <summary>
/// <para>Sets the actor as invisible, adds a point to the other player</para>
/// <para>Sets Dead to True</para>
/// <para>Sets a cowntdown for respawn</para>
/// </summary>
/// <param name="OtherPlayer"></param>
void APlayerCharacter::Die(AActor *OtherPlayer)
{
	if (GEngine) {
		SetActorHiddenInGame(true);
		APlayerCharacter* OtherP = Cast<APlayerCharacter>(OtherPlayer);
		OtherP->GainPoint();
		GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &APlayerCharacter::Respawn, RespawnTime, false);
		weapon->Hide();
		
		Dead = true;
	}
}
/// <summary>
/// <para>Resets the player's health</para>
/// <para>sets the actor to visible</para>
/// <para>shows the weapon</para>
/// <para>sets the dead bool to false</para>
/// </summary>
void APlayerCharacter::Respawn()
{
	Health = MaxHealth;
	SetActorLocation(SpawnLocation);
	SetActorHiddenInGame(false);
	weapon->Show();
	Dead = false;
}

//adds 1 point to the player
void APlayerCharacter::GainPoint()
{
	points++;
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green,FString::FromInt(points));
	}
}

//Returns Movement Vector
FVector2D APlayerCharacter::GetMovementVector() const
{
	return FVector2D(MovementVector);
}
/// <summary>
/// <para>Draws 3 lines used to calculate the vault location</para>
/// <para>First one checks if there is a wall within vaulting distance</para>
/// second checks if the wall has a ledge to vault to
/// The third checks the wall's normal, and decides whether the player can vault
/// </summary>
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

float APlayerCharacter::GetMaxHealth() const
{
	return MaxHealth;
}

int APlayerCharacter::GetScore() const
{
	return points;
}

float APlayerCharacter::GetCurrentHealth() const
{
	return Health;
}
