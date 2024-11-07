// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterGameModeBase.h"

void AShooterGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld(); // Returns the world the actor is in
	UGameInstance* gameInstance = World->GetGameInstance(); //keeps track of game
	FString error = "Players not found!";
	//Create local multiplayer mode
	ULocalPlayer* LocalPlayer = World->GetGameInstance()->CreateLocalPlayer(1, error, true);
}