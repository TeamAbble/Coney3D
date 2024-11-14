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
		collider->OnComponentHit.AddDynamic(this, &AWeaponProjectile::OnHit);
		collider->OnComponentBeginOverlap.AddDynamic(this, &AWeaponProjectile::BeginOverlap);
		collider->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	}
	movement = GetComponentByClass<UProjectileMovementComponent>();	
}

// Called every frame
void AWeaponProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	distanceTravelled += DeltaTime * GetVelocity().Size();

	if (distanceTravelled >= expiryDistance) {

		Explode();
	}
}

void AWeaponProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!ActorOwner)
		return;
	DealDamage(HitComponent, OtherActor, OtherComponent, Hit);
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 0.3f, FColor::Red, ActorOwner->GetName());
	}
}

void AWeaponProjectile::DealDamage(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, const FHitResult& Hit) {
	if (OtherActor) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 0.3f, FColor::Yellow, "Bullet hit " + OtherActor->GetName());
		}
		if (OtherActor != ActorOwner && OtherActor != this) {
			if (OtherComponent) {
				if (OtherComponent->IsSimulatingPhysics()) {
					OtherComponent->AddImpulseAtLocation(movement->Velocity * collisionImpulseMultiplier, GetActorLocation());
				}
			}
			if (OtherActor->CanBeDamaged()) {

				if (useRadialDamage) {
					UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), maxRadialDamage, minRadialDamage, GetActorLocation(), minDamageRadius, maxDamageRadius, 1, damageType, 
						TArray<AActor*,FDefaultAllocator>(), ActorOwner);
				}
				else {
					float damageDealt = FMath::Lerp(maxDamage, minDamage, FMath::Clamp(UKismetMathLibrary::NormalizeToRange(distanceTravelled, minRange, maxRange), 0, 1));
					UGameplayStatics::ApplyDamage(OtherActor, damageDealt, GetInstigatorController(), ActorOwner, damageType);
				}
			}
		}
	}
	Explode();
}

void AWeaponProjectile::Explode()
{
	if (hitEffect) {
		FActorSpawnParameters params = FActorSpawnParameters();
		params.Owner = this;
		params.Instigator = GetInstigator();
		FTransform transform = FTransform(GetActorRotation(), GetActorLocation());
		AActor* projectile = GetWorld()->SpawnActor<AActor>(hitEffect, params);
	}
	Destroy();
}

void AWeaponProjectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!ActorOwner)
		return;
	DealDamage(OverlappedComponent, OtherActor, OtherComp, SweepResult);
}

AActor* AWeaponProjectile::GetActorOwner() const
{
	return ActorOwner;
}




