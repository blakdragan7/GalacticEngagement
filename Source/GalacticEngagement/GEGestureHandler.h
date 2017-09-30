// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GEGestureHandler.generated.h"

/* Delegate For Receiving Gesture Events */

class GALACTICENGAGEMENT_API IGEGestureHandlerDelegate
{
public:
	virtual void DoubleTap(float x, float y) = 0;
	virtual void SingleTap(float x, float y) = 0;
	virtual void ConstTapStart(float x, float y) = 0;
	virtual void ConstTapEnd(float x, float y) = 0;
	virtual void SwipeStart(float x, float y) = 0;
	virtual void SwipeUpdate(float x, float y) = 0;
	virtual void SwipeEnd(float x, float y) = 0;
};

/**
 *  Handles Touch Gestures with callbacks
 *	This Class Is Thread Safe
 */
UCLASS()
class GALACTICENGAGEMENT_API UGEGestureHandler : public UObject
{
	GENERATED_BODY()
	
private:
	TArray<IGEGestureHandlerDelegate*> delegates;

	FCriticalSection CRSection;
	
	float TimeToTap;
	float MaxTapTime;

	bool HasTouchDown;
	bool SwipeStarted;
	bool HasConstTap;

	float LastTouchDown,LastTouchUp;
	int32 TapCount; // Number of consistent taps

private:
	void TappOccured(float x, float y);

public:

	UGEGestureHandler();

	virtual void Tick(float DeltaTime,float x,float y);

	void RegisterDelegate(IGEGestureHandlerDelegate* _delegate);
	void UnregisterDelegate(IGEGestureHandlerDelegate* _delegate);

	void TouchDown(float x, float y); // Called Tp Update The State Of Gestures
	void TouchUp(float x,float y); // Called Tp Update The State Of Gestures
};