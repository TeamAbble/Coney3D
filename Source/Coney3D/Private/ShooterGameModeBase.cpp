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
	GetWorld()->GetTimerManager().SetTimer(RoundTimer, this, &AShooterGameModeBase::EndMatch, RoundTimeInSeconds, false);
}

FString AShooterGameModeBase::MinutesSeconds(float seconds)
{
	int Minutes = FMath::FloorToInt(RoundTimeInSeconds / 60);
	float Seconds = RoundTimeInSeconds;
	return FString(FString::FromInt(Minutes)+ FString(":")+FString::SanitizeFloat(Seconds));
}

void AShooterGameModeBase::EndMatch()
{
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "Match Ended");
		int CurrentWinnerIndex=-1;
		int CurrentBestScore=-1;
		TArray<ULocalPlayer*> LocalPlayers = GetGameInstance()->GetLocalPlayers();
		for (int i = 0; i < sizeof(LocalPlayers); i++) {
			APlayerCharacter* Player = Cast<APlayerCharacter>(LocalPlayers[i]);
			//if (CurrentBestScore<Player->) {

			//}

		}
		
	}
}

