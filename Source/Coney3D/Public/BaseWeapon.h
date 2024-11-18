// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "EFireType.h"
#include "PlayerCharacter.h"
#include "WeaponProjectile.h"
#include "Animation/AnimSequence.h"
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
	/// <summary>
	/// performs a line trace with some mildly annoying maths to shoot a target
	/// </summary>
	void TryFire();
	/// <summary>
	/// Uses information from the linetrace performed by TryFire() to spawn and set up a tracer.
	/// </summary>
	/// <param name="traceStart"></param>
	/// <param name="traceEnd"></param>
	void CreateProjectile(FVector traceStart, FVector traceEnd);
	void ResetFired();
	/// <summary>
	/// Is this weapon currently able to fire?
	/// </summary>
	bool canFire = true;
	/// <summary>
	/// Has the weapon been fired?
	/// </summary>
	bool fired = false;
	/// <summary>
	/// Is firing this weapon currently blocked?
	/// </summary>
	bool fireBlocked = false;
	/// <summary>
	/// How much ammunition we currently have
	/// </summary>
	int32 currentAmmo = 0;
	/// <summary>
	/// How much ammo we have when full. If zero, we won't use ammo
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon - Ammunition") int32 maxAmmo = 0;
	/// <summary>
	/// How many times a minute does this weapon fire?
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Operation") float shotsPerMinute = 600;
	/// <summary>
	/// The time, in seconds, between shots
	/// </summary>
	float timeBetweenShots = 0;
	/// <summary>
	/// How long the weapon must be charged for to fire
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Operation") float chargeTime = 0;
	/// <summary>
	/// How quickly the charge dissipates when not trying to fire
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Operation") float chargeDecay = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Operation", meta = (Tooltip = "How many times the weapon fires, - 1. The maximum number of iterations per shot is 256.")) uint8 fireIterations = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Operation") bool fullChargeOnFire = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Operation") TEnumAsByte<EFireType> fireMode = EFireType::rapidfire;
	float currentCharge;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Accuracy") float maxAccumulatedSpreadAngle = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Accuracy") float accumulatedSpeadPerShot = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Accuracy") float accumulatedSpeadCurrent = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Accuracy") float accumulatedSpreadDecay = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Accuracy") float hipFireSpreadAngle = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Accuracy") float crosshairSpreadCoefficient = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Accuracy") float maxRange = 10000.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Accuracy") float minRange = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Damage") float maxDamage = 12.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Damage") float minDamage = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Damage") float collisionImpulseMultiplier = 0.f;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	bool fireInput;
	UPROPERTY(EditAnywhere, Category = "Weapon - References", Replicated) APlayerCharacter* connectedPlayer;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,  Category = "Weapon - References") USceneComponent* muzzlePoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon - References") FVector muzzlePointPosition;
	UPROPERTY(EditAnywhere, Category = "Weapon - Visuals") UNiagaraSystem* muzzleFlashSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon - Visuals") UNiagaraComponent* muzzleFlashInstance;

	UPROPERTY(EditAnywhere, Category = "Collision")
	TEnumAsByte<ECollisionChannel> traceChannelProperty = ECC_Pawn;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon - Visuals") TSubclassOf<class AWeaponProjectile> projectileBlueprint;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Visuals") float projectileSpeed = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Visuals") float tracerDeleteTime = 2.f;

	bool bActive;
	void Hide();
	void Show();
	bool firePressed;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon - Aiming") float aimSpeed = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon - Aiming") float aimFOV = 60;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Animation") UAnimSequence* idleAnim;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Animation") UAnimMontage* fireAnim;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Animation") UAnimMontage* reloadAnim;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Animation") UAnimMontage* equipAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Animation") UAnimSequence* characterIdleAnim;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Animation") UAnimMontage* characterFireAnim;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Animation") UAnimMontage* characterReloadAnim;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon - Animation") UAnimMontage* characterEquipAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon - Animation") USkeletalMeshComponent* weaponMesh;


};
