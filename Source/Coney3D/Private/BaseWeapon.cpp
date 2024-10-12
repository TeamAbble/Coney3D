// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseWeapon.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	timeBetweenShots = 1 / (shotsPerMinute / 60);
	if (GEngine) {
		//Lol
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, "Set Time Between rounds on " + GetName() + " to " + FString::SanitizeFloat(timeBetweenShots));
	}
	connectedPlayer = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	canFire = true;
	currentAmmo = maxAmmo;
}

void ABaseWeapon::TryFire()
{
	if (GEngine) {
		//Lol
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, "Fired Weapon: " + GetName());
	}
	if (muzzleFlashInstance) {
		muzzleFlashInstance->Activate();
	}
	fired = true;
	GetWorld()->GetTimerManager().SetTimer(resetFireTimerHandle, this, &ABaseWeapon::ResetFired, timeBetweenShots,false);
}

void ABaseWeapon::ResetFired()
{
	fired = false;
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	canFire = !fireBlocked;
	bool firePressed = false;
	if (!connectedPlayer) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, "ERROR! NO PLAYER CHARACTER OWNS WEAPON: " + GetName());
		}
	}
	if (fireMode == none) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, "FIREMODE IS SET TO NONE ON WEAPON: " + GetName());
		}
	}
	else {
		fireInput = connectedPlayer->GetFireInput();
	}
	

	if (canFire) {
		if (chargeTime > 0) {
			currentCharge = FMath::Clamp(currentCharge + (fireInput ? DeltaTime : -chargeDecay), 0, chargeTime);
		}

		if (fireInput && chargeTime > 0) {
			if (GEngine) {
				GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, "Weapon charge: " + FString::SanitizeFloat(currentCharge));
			}
		}

		if (!fired && fireInput && (chargeTime == 0 || currentCharge >= chargeTime) && (firePressed || fireMode != single)) {
			TryFire();
			firePressed = true;
		}
	}
	else {
		currentCharge = FMath::Min(currentCharge - chargeDecay, 0);
	}
}

