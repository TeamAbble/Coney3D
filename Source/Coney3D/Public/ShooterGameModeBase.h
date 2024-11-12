// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlayerCharacter.h"
#include "ShooterGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class CONEY3D_API AShooterGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	

public:
protected: 
	virtual void BeginPlay() override;

public: 
	FTimerHandle RoundTimer;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float RoundTimeInSeconds = 500;
	UPROPERTY(BlueprintReadOnly)FString WinnerText;
	FString MinutesSeconds(float seconds);
	void EndMatch();
	

};
