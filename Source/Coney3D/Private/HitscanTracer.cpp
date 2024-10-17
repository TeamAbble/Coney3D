// Fill out your copyright notice in the Description page of Project Settings.


#include "HitscanTracer.h"

// Sets default values
AHitscanTracer::AHitscanTracer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHitscanTracer::BeginPlay()
{
	Super::BeginPlay();
	
}
// Called every frame
void AHitscanTracer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetRemoteRole() != ENetRole::ROLE_Authority)
		return;
	lifetime += distanceIncrement * DeltaTime;
	SetActorLocation(FMath::Lerp<FVector, float>(start, end, FMath::Clamp(lifetime, 0, 1)));
	
	if (lifetime >= 1 + additionalLifetime && !destroyed) {
		Destroy();
		destroyed = true;
	}
}

