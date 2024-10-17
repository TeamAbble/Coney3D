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
			if (GEngine) {
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, "Hit actor: " + hit.GetActor()->GetName());
			}
			UE_LOG(LogTemp, Display, TEXT("actor hit: %s"), *hit.GetActor()->GetName());
		}
		else {
			if (GEngine) {
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, "Did not hit actor");
			}
			UE_LOG(LogTemp, Display, TEXT("Nothing hit"));
		}
		accumulatedSpeadCurrent += accumulatedSpeadPerShot;
		CreateTracer(muzzlePoint->GetComponentLocation(), hit.bBlockingHit ? hit.ImpactPoint : traceEnd);
	}

}
void ABaseWeapon::CreateTracer(FVector traceStart, FVector traceEnd)
{
	FActorSpawnParameters parameters = FActorSpawnParameters();
	parameters.Owner = this;
	parameters.Instigator = GetInstigator();
	FTransform transform = FTransform();
	transform.SetLocation(traceStart);
	transform.SetRotation(FQuat());
	AHitscanTracer* ht = GetWorld()->SpawnActor<AHitscanTracer>(projectileBlueprint, transform, parameters);
	ht->start = traceStart;
	ht->end = traceEnd;
	ht->additionalLifetime = tracerDeleteTime;
	ht->distanceIncrement = tracerSpeed / FVector::Distance(traceStart, traceEnd);
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
			GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, "ERROR! NO PLAYER CHARACTER OWNS WEAPON: " + GetName());
		}
	}
	if (fireMode == none) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, "FIREMODE IS SET TO NONE ON WEAPON: " + GetName());
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
			TryFire();
			firePressed = true;
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

