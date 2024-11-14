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
	//GetWorld()->GetTimerManager().SetTimer(RoundTimer, this, &AShooterGameModeBase::EndMatch, RoundTimeInSeconds, false);
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
	}

	int CurrentWinnerIndex=-1;
	int CurrentBestScore=-1;
	bool bDraw = false;
	TArray<ULocalPlayer*> LocalPlayers = GetGameInstance()->GetLocalPlayers();
	for (int i = 0; i < LocalPlayers.Num(); i++) {
		if (LocalPlayers[i] != nullptr) {
			APlayerCharacter* Player = Cast<APlayerCharacter>(LocalPlayers[i]->GetPlayerController(GetWorld())->GetCharacter());
			if (Player && Player->GetScore() > CurrentBestScore) {
				CurrentBestScore = Player->GetScore();
				CurrentWinnerIndex = LocalPlayers[i]->GetIndexInGameInstance();
				bDraw = false;
			}
			else if (Player && Player->GetScore() == CurrentBestScore) {
				bDraw = true;
			}
			Player->Dead = true;
				
		}
	}
	if (!bDraw) {
		WinnerText = FString("Player ") + FString::FromInt(CurrentWinnerIndex + 1) + FString(" wins with ") + FString::FromInt(CurrentBestScore) + FString(" points");
	}
	else {
		WinnerText = FString("Draw");
	}


}

