// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterGameModeBase.h"
#include "Kismet/GameplayStatics.h"


void AShooterGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld(); // Returns the world the actor is in
	UGameInstance* gameInstance = World->GetGameInstance(); //keeps track of game
	FString error = "Players not found!";
	//Create local multiplayer mode
	ULocalPlayer* LocalPlayer = World->GetGameInstance()->CreateLocalPlayer(1, error, true);
	APlayerCharacter* playerOne = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (playerOne) {
		PlayerOne = playerOne;
	}
	APlayerCharacter* newCharacter = Cast<APlayerCharacter>(LocalPlayer->GetPlayerController(GetWorld())->GetCharacter());
	if (newCharacter) {
		PlayerTwo;
	}
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
	//We now get the players at the start of the game, so this block is redundant. It has been re-implemented below.

	//TArray<ULocalPlayer*> LocalPlayers = GetGameInstance()->GetLocalPlayers();
	//for (int i = 0; i < LocalPlayers.Num(); i++) {
	//	if (LocalPlayers[i] != nullptr) {
	//		APlayerCharacter* Player = Cast<APlayerCharacter>(LocalPlayers[i]->GetPlayerController(GetWorld())->GetCharacter());
	//		if (Player)
	//		{
	//			if (Player->GetScore() > CurrentBestScore)
	//			{
	//				CurrentBestScore = Player->GetScore();
	//				CurrentWinnerIndex = LocalPlayers[i]->GetIndexInGameInstance();
	//				bDraw = false;
	//			}
	//			else if (Player && Player->GetScore() == CurrentBestScore) {
	//				bDraw = true;
	//			}
	//			Player->Dead = true;
	//		}
	//	}
	//}
	int score1 = PlayerOne->GetScore();
	int score2 = PlayerTwo->GetScore();
	if (score1 == score2) {
		bDraw = true;
	}
	else {
		bool p1Wins = score1 > score2;
		CurrentWinnerIndex = p1Wins ? 0 : 1;
		CurrentBestScore = p1Wins ? score1 : score2;
	}
	if (!bDraw) {
		WinnerText = FString("Player ") + FString::FromInt(CurrentWinnerIndex + 1) + FString(" wins with ") + FString::FromInt(CurrentBestScore) + FString(" points");
	}
	else {
		WinnerText = FString("Draw");
	}
}




