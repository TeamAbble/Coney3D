// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpPad.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AJumpPad::AJumpPad()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	//initialise the mesh
	JumpPadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Jump Pad Mesh"));

	jumpPadDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("Jump Direction"));

	JumpPadTriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Jump Pad Trigger Volume"));
	JumpPadTriggerVolume->SetupAttachment(JumpPadMesh);
}

// Called when the game starts or when spawned
void AJumpPad::BeginPlay()
{
	Super::BeginPlay();

	JumpPadMesh->SetMaterial(0, InactiveMaterial);

	JumpPadTriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AJumpPad::JumpPadTriggered);
	JumpPadTriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AJumpPad::JumpPadEndTrigger);
}

// Called every frame
void AJumpPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJumpPad::JumpPadTriggered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || !OtherComp)
		return;

	/*if (APlayerCharacter* ourChar = Cast<APlayerCharacter>(OtherActor))
	{
		if (Cast<UCapsuleComponent>(OtherComp)) == ourChar->GetCapsuleComponent())
		{
			JumpPadMesh->SetMaterial(0, ActiveMaterial);
			ourChar->GetCharacterMovement()->JumpZVelocity *= 2;
		}
	}*/
	//What about if, instead, we did this?
	FVector force = jumpPadDirection->GetForwardVector() * launchForce;
	if (OtherComp->IsSimulatingPhysics()) {
		if (setVelocityInsteadOfForce) {
			OtherComp->SetPhysicsLinearVelocity(force);
		}
		else {
			OtherComp->AddForce(force);
		}
		return;
	}
	ACharacter* character = Cast<ACharacter>(OtherActor);
	if (character) {
		if (setVelocityInsteadOfForce) {
			character->LaunchCharacter(force, true, true);
		}
		else
		{
			character->LaunchCharacter(force, false, false);
		}
	}
}

void AJumpPad::JumpPadEndTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || !OtherComp)
		return;

	/*if (APlayerCharacter* ourChar = Cast<APlayerCharacter>(OtherActor))
	{
		if (Cast<UCapsuleComponent>(OtherComp)) == ourChar->GetCapsuleComponent())
		{
			JumpPadMesh->SetMaterial(0, InactiveMaterial);
			ourChar->GetCharacterMovement()->JumpZVelocity /= 2;
		}
	}*/
}

