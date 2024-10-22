// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AWeaponProjectile::AWeaponProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWeaponProjectile::BeginPlay()
{
	Super::BeginPlay();
	distanceTravelled = 0;
	collider = GetComponentByClass<USphereComponent>();
	if (collider) {
		if (GIsServer) {
			collider->OnComponentHit.AddDynamic(this, &AWeaponProjectile::OnHit);
		}
		collider->SetCollisionProfileName(TEXT("Projectile"));
	}
	movement = GetComponentByClass<UProjectileMovementComponent>();
	if (movement) {
		if (!GIsServer) {
			movement->ProjectileGravityScale = 0;
		}
	}
	
}

// Called every frame
void AWeaponProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	distanceTravelled += DeltaTime * GetVelocity().Size();

	if (distanceTravelled >= expiryDistance) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, "Projectile expired");
		}
		Explode();
	}
}

void AWeaponProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!GIsServer || !movement || !ActorOwner || !OtherActor || !OtherComponent || !HitComponent)
		return;

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, "Bullet hit " + OtherActor->GetName());
	}

	if (OtherActor != ActorOwner && OtherActor != this) {
		if (OtherComponent->IsSimulatingPhysics()) {
			OtherComponent->AddImpulseAtLocation(movement->Velocity * collisionImpulseMultiplier, GetActorLocation());
		}
		if (OtherActor->CanBeDamaged()) {

			float damageDealt = FMath::Lerp(maxDamage, minDamage, UKismetMathLibrary::NormalizeToRange(distanceTravelled, minRange, maxRange));
			UGameplayStatics::ApplyDamage(OtherActor, damageDealt, GetInstigatorController(), ActorOwner, damageType);
		}
		Explode();
	}
}

void AWeaponProjectile::Explode()
{
	Destroy();
}



