// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "GameViewportFixClient.generated.h"

/**
 * 
 */
UCLASS()
class CONEY3D_API UGameViewportFixClient : public UGameViewportClient
{
	GENERATED_BODY()
public:
	virtual bool InputKey(const FInputKeyEventArgs& EventArgs) override;
	virtual void RemapControllerInput(FInputKeyEventArgs& InOutKeyEvent) override;

	virtual bool InputAxis(FViewport* InViewport, FInputDeviceId InputDevice, FKey Key, float Delta, float DeltaTime, int32 NumSamples = 1, bool bGamepad = false) override;

private:
	ULocalPlayer* GetLocalPlayerFromControllerId(int32 ControllerId) const;
	/** A broadcast delegate broadcasting from UGameViewportClient::InputKey */
	FOnInputKeySignature OnInputKeyEvent;

	/** Delegate for overriding input key behavior */
	FOverrideInputKeyHandler OnOverrideInputKeyEvent;

	/** A broadcast delegate broadcasting from UGameViewportClient::InputAxis */
	FOnInputAxisSignature OnInputAxisEvent;

	/** Delegate for overriding input axis behavior */
	FOverrideInputAxisHandler OnOverrideInputAxisEvent;

#if WITH_EDITOR
	/** Delegate called when game viewport client received input key */
	FOnGameViewportInputKey GameViewportInputKeyDelegate;
#endif
};

