// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HitscanTracer.generated.h"

UCLASS()
class CONEY3D_API AHitscanTracer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHitscanTracer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector start = FVector(), end = FVector();
	float distanceIncrement = 0, lifetime = 0, additionalLifetime = 0;
	bool destroyed = false;

};
