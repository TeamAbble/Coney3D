// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"

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
}

void ABaseWeapon::TryFire()
{
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, "Fired Weapon: " + GetName());
	}
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	bool firePressed = false;

	if (canFire) {
		if (chargeTime > 0) {
			currentCharge = FMath::Clamp(currentCharge + (fireInput ? DeltaTime : -chargeDecay), 0, chargeTime);
		}

		if (fireInput && (chargeTime == 0 || currentCharge >= chargeTime) && (firePressed || fireMode != FireType::single)) {
			TryFire();
			firePressed = true;
		}
	}
	else {
		currentCharge = FMath::Min(currentCharge - chargeDecay, 0);
	}
}

