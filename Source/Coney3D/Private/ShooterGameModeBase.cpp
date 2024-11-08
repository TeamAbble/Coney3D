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
	GetWorld()->GetTimerManager().SetTimer(RoundTimer, this, AShooterGameModeBase::EndMatch, RoundTimeInSeconds, false);
}

FString AShooterGameModeBase::MinutesSeconds(float seconds)
{
	float Minutes;
	float Seconds = RoundTimeInSeconds;
	while (RoundTimeInSeconds > 60) {
		seconds -= 60;
		Minutes++;
	}

	return FString(FString::SanitizeFloat(Minutes)+ FString(":")+FString::SanitizeFloat(Seconds));
}

void AShooterGameModeBase::EndMatch()
{
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "Match Ended");
	}
}

