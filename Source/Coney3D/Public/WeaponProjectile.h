// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "WeaponProjectile.generated.h"

UCLASS()
class CONEY3D_API AWeaponProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	float maxDamage = 0, minDamage = 0, minRange = 0, maxRange = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) USphereComponent* collider;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) UProjectileMovementComponent* movement;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) AActor* ActorOwner;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool useRadialDamage;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float minRadialDamage;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)	float maxRadialDamage;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)	float minDamageRadius;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)	float maxDamageRadius;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TSubclassOf<UDamageType> damageType;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float distanceTravelled;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float expiryDistance = 100000;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TSubclassOf<AActor> hitEffect;
	float collisionImpulseMultiplier;
	float projectileSpeed;
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void DealDamage(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, const FHitResult& Hit);
	UFUNCTION()
	virtual void Explode();
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};
