// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
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
	canFire = true;
	bReplicates = true;
	currentAmmo = maxAmmo;
}

void ABaseWeapon::TryFire_Implementation()
{
	if (GEngine) {
		//Lol
	}

	//lets shoot shit
	for (size_t i = 0; i < static_cast<int16>(fireIterations + 1); i++)
	{
		//We need an FHitResult, like a raycast hit
		FHitResult hit;
		//Then set the start and end points
		FVector traceStart = connectedPlayer->playerCam->GetComponentLocation();
		FVector traceEnd = connectedPlayer->playerCam->GetComponentLocation() + (FMath::VRandCone(connectedPlayer->playerCam->GetForwardVector(), 
			FMath::DegreesToRadians(accumulatedSpeadCurrent + hipFireSpreadAngle))) * maxRange;

		//Then create query parameters, so we can ignore the player and the weapon
		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(this);
		queryParams.AddIgnoredActor(connectedPlayer);

		//Then we run the line trace
		GetWorld()->LineTraceSingleByChannel(hit, traceStart, traceEnd, traceChannelProperty, queryParams);
		//Draw a debug line for it
		//DrawDebugLine(GetWorld(), traceStart, traceEnd, hit.bBlockingHit ? FColor::Green : FColor::Red, false, .5f, 0, 2);

		if (hit.bBlockingHit && IsValid(hit.GetActor())) {
			UE_LOG(LogTemp, Display, TEXT("actor hit: %s"), *hit.GetActor()->GetName());
		}
		else {
			UE_LOG(LogTemp, Display, TEXT("Nothing hit"));
		}
		accumulatedSpeadCurrent += accumulatedSpeadPerShot;
		CreateProjectile(muzzlePoint->GetComponentLocation(), hit.bBlockingHit ? hit.ImpactPoint : traceEnd);
	}

}


void ABaseWeapon::CreateProjectile(FVector traceStart, FVector traceEnd)
{
	//Changed from hitscan weapon to projectile weapon.
	//The parameters will be kept for other weapons that might need it later on.
	FActorSpawnParameters parameters = FActorSpawnParameters();
	parameters.Owner = this;
	parameters.Instigator = GetInstigator();
	FTransform transform = FTransform();
	transform.SetLocation(traceStart);
	transform.SetRotation((traceEnd - traceStart).Rotation().Quaternion());
	AWeaponProjectile* ht = GetWorld()->SpawnActor<AWeaponProjectile>(projectileBlueprint, transform, parameters);
	ht->projectileSpeed = projectileSpeed;
	ht->ActorOwner = connectedPlayer;
	ht->maxRange = maxRange;
	ht->minRange = minRange;
	ht->maxDamage = maxDamage;
	ht->minDamage = minDamage;
	ht->collisionImpulseMultiplier = collisionImpulseMultiplier;
	if (ht->movement) {
		ht->movement->Velocity = ht->GetActorForwardVector() * projectileSpeed;
	}
	UE_LOG(LogTemp, Display, TEXT("created actor spawn parameters"));
	UE_LOG(LogTemp, Display, TEXT("added tracer to vector"));
}

void ABaseWeapon::ResetFired()
{
	fired = false;
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//Now we can process the tracers
	canFire = !fireBlocked;
	bool firePressed = false;
	if (!connectedPlayer) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::Red, "ERROR! NO PLAYER CHARACTER OWNS WEAPON: " + GetName());
		}
	}
	if (fireMode == none) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::Red, "FIREMODE IS SET TO NONE ON WEAPON: " + GetName());
		}
	}
	else {
		if(connectedPlayer)
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
			if (muzzleFlashInstance) {
				muzzleFlashInstance->Activate(true);
			}
			fired = true;
			TryFire();
			firePressed = true;
			GetWorld()->GetTimerManager().SetTimer(resetFireTimerHandle, this, &ABaseWeapon::ResetFired, timeBetweenShots, false);

		}
	}
	else {
		currentCharge = FMath::Min(currentCharge - chargeDecay, 0);
	}

	//if(accumulatedSpeadCurrent > 0)
	//	accumulatedSpeadCurrent -= DeltaTime * accumulatedSpreadDecay;
	//else if (accumulatedSpeadCurrent < 0) {
	//	accumulatedSpeadCurrent = 0;
	//}

	accumulatedSpeadCurrent = FMath::Clamp(accumulatedSpeadCurrent - DeltaTime * accumulatedSpreadDecay, 0, maxAccumulatedSpreadAngle);
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseWeapon, connectedPlayer);
}

