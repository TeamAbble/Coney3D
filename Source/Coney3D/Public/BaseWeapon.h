// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "EFireType.h"
#include "PlayerCharacter.h"
#include "BaseWeapon.generated.h"

UCLASS()
class CONEY3D_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle resetFireTimerHandle;

public:	

	void TryFire();
	void ResetFired();
	/// <summary>
	/// Is this weapon currently able to fire?
	/// </summary>
	bool canFire;
	/// <summary>
	/// Has the weapon been fired?
	/// </summary>
	bool fired;
	/// <summary>
	/// Is firing this weapon currently blocked?
	/// </summary>
	bool fireBlocked;
	/// <summary>
	/// How much ammunition we currently have
	/// </summary>
	int32 currentAmmo;
	/// <summary>
	/// How much ammo we have when full. If zero, we won't use ammo
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon - Ammunition") int32 maxAmmo;
	/// <summary>
	/// How many times a minute does this weapon fire?
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Operation") float shotsPerMinute;
	/// <summary>
	/// The time, in seconds, between shots
	/// </summary>
	float timeBetweenShots;
	/// <summary>
	/// How long the weapon must be charged for to fire
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Operation") float chargeTime;
	/// <summary>
	/// How quickly the charge dissipates when not trying to fire
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Operation") float chargeDecay;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Operation") bool fullChargeOnFire;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Operation") TEnumAsByte<EFireType> fireMode;
	float currentCharge;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Accuracy") float maxAccumulatedSpreadAngle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Accuracy") float max;


	// Called every frame
	virtual void Tick(float DeltaTime) override;
	bool fireInput;
	UPROPERTY(EditAnywhere, Category = "Weapon - References") APlayerCharacter* connectedPlayer;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,  Category = "Weapon - References") USceneComponent* muzzlePoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon - References") FVector muzzlePointPosition;
	UPROPERTY(EditAnywhere, Category = "Weapon - Visuals") UNiagaraSystem* muzzleFlashSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon - Visuals") UNiagaraComponent* muzzleFlashInstance;

};
