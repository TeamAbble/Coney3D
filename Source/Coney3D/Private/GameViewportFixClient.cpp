// Fill out your copyright notice in the Description page of Project Settings.


#include "GameViewportFixClient.h"
#include "GameMapsSettings.h"
#include "Engine/Console.h"
#include "PlayerCharacter.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/PlayerInput.h"

bool UGameViewportFixClient::InputKey(const FInputKeyEventArgs& InEventArgs)
{
	FInputKeyEventArgs EventArgs = InEventArgs;
	if (TryToggleFullscreenOnInputKey(EventArgs.Key, EventArgs.Event))
	{
		return true;
	}
	if (EventArgs.Key == EKeys::LeftMouseButton && EventArgs.Event == EInputEvent::IE_Pressed)
	{
		GEngine->SetFlashIndicatorLatencyMarker(GFrameCounter);
	}

	//if turn on skip setting and using gamepad, increment controllerId
	RemapControllerInput(EventArgs);
	if (IgnoreInput())
	{
		return ViewportConsole ? ViewportConsole->InputKey(EventArgs.InputDevice, EventArgs.Key, EventArgs.Event, EventArgs.AmountDepressed, EventArgs.IsGamepad()) : false;
	}
	OnInputKeyEvent.Broadcast(EventArgs);

#if WITH_EDITOR
	// Give debugger commands a chance to process key binding
	if (GameViewportInputKeyDelegate.IsBound())
	{
		if (GameViewportInputKeyDelegate.Execute(EventArgs.Key, FSlateApplication::Get().GetModifierKeys(), EventArgs.Event))
		{
			return true;
		}
	}
#endif

	// route to subsystems that care
	bool bResult = (ViewportConsole ? ViewportConsole->InputKey(EventArgs.InputDevice, EventArgs.Key, EventArgs.Event, EventArgs.AmountDepressed, EventArgs.IsGamepad()) : false);
	// Try the override callback, this may modify event args
	if (!bResult && OnOverrideInputKeyEvent.IsBound())
	{
		bResult = OnOverrideInputKeyEvent.Execute(EventArgs);
	}
	if (!bResult)
	{
		ULocalPlayer* TargetLocalPlayer = GetLocalPlayerFromControllerId(EventArgs.ControllerId);
		if (TargetLocalPlayer && TargetLocalPlayer->PlayerController)
		{
			bResult = TargetLocalPlayer->PlayerController->InputKey(FInputKeyParams(EventArgs.Key, EventArgs.Event, static_cast<double>(EventArgs.AmountDepressed), EventArgs.IsGamepad(), EventArgs.InputDevice));
		}

		// A gameviewport is always considered to have responded to a mouse buttons to avoid throttling
		if (!bResult && EventArgs.Key.IsMouseButton())
		{
			bResult = true;
		}
	}
	return bResult;

}

void UGameViewportFixClient::RemapControllerInput(FInputKeyEventArgs& InOutEventArgs)
{
	const int32 NumLocalPlayers = World ? World->GetGameInstance()->GetNumLocalPlayers() : 0;

	if (NumLocalPlayers > 1 && InOutEventArgs.Key.IsGamepadKey() && GetDefault<UGameMapsSettings>()->bOffsetPlayerGamepadIds)
	{
		InOutEventArgs.ControllerId++;
	}
	else if (InOutEventArgs.Viewport->IsPlayInEditorViewport() && InOutEventArgs.Key.IsGamepadKey())
	{
		GEngine->RemapGamepadControllerIdForPIE(this, InOutEventArgs.ControllerId);
	}
}

bool UGameViewportFixClient::InputAxis(FViewport* InViewport, FInputDeviceId InputDevice, FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
{
	if (IgnoreInput())
	{
		return false;
	}

	// Handle mapping controller id and key if needed
	FInputKeyEventArgs EventArgs(InViewport, InputDevice, Key, IE_Axis);

	//if turn on skip setting and using gamepad, increment controllerId
	RemapControllerInput(EventArgs);

	OnInputAxisEvent.Broadcast(InViewport, EventArgs.ControllerId, EventArgs.Key, Delta, DeltaTime, NumSamples, EventArgs.IsGamepad());

	bool bResult = false;

	if (!(InViewport->IsSlateViewport() && InViewport->IsPlayInEditorViewport()) || ViewportConsole == nullptr || !ViewportConsole->ConsoleActive())
	{
		// route to subsystems that care
		if (ViewportConsole != nullptr)
		{
			bResult = ViewportConsole->InputAxis(EventArgs.InputDevice, EventArgs.Key, Delta, DeltaTime, NumSamples, EventArgs.IsGamepad());
		}

		// Try the override callback, this may modify event args
		if (!bResult && OnOverrideInputAxisEvent.IsBound())
		{
			bResult = OnOverrideInputAxisEvent.Execute(EventArgs, Delta, DeltaTime, NumSamples);
		}

		if (!bResult)
		{
			ULocalPlayer* const TargetLocalPlayer = GetLocalPlayerFromControllerId(EventArgs.ControllerId);
			if (TargetLocalPlayer && TargetLocalPlayer->PlayerController)
			{
				bResult = TargetLocalPlayer->PlayerController->InputKey(FInputKeyParams(EventArgs.Key, (double)Delta, DeltaTime, NumSamples, EventArgs.IsGamepad(), EventArgs.InputDevice));
			}
		}

		if (InViewport->IsSlateViewport() && InViewport->IsPlayInEditorViewport())
		{
			// Absorb all keys so game input events are not routed to the Slate editor frame
			bResult = true;
		}
	}
	return bResult;
}

ULocalPlayer* UGameViewportFixClient::GetLocalPlayerFromControllerId(int32 ControllerId) const
{
	if (UGameInstance* BoozeGameInstance = GetWorld()->GetGameInstance())
	{
		const TArray<ULocalPlayer*>& BoozeLocalPlayerArray = BoozeGameInstance->GetLocalPlayers();
		for (ULocalPlayer* const BoozeLocalPlayer : BoozeLocalPlayerArray)
		{
			if (BoozeLocalPlayer && BoozeLocalPlayer->GetControllerId() == ControllerId)
			{
				return BoozeLocalPlayer;
			}
		}
	}
	return nullptr;
}
