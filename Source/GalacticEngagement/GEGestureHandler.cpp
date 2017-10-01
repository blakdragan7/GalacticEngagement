// Fill out your copyright notice in the Description page of Project Settings.

#include "GEGestureHandler.h"

#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

#define LOG(text) UE_LOG(LogTemp,Warning,TEXT(text))

UGEGestureHandler::UGEGestureHandler()
{
	LastTouchDown	= 0;
	LastTouchUp		= 0;
	TapCount		= 0;

	HasTouchDown	= false;
	SwipeStarted	= false;
	HasConstTap		= false;
	HasAnyTouch		= false;

	TimeToTap		= 0.11f;
	MaxTapTime		= 0.12f;
}

void UGEGestureHandler::Tick(float DeltaTime,float x,float y)
{
	FScopeLock lock(&mutex);
	if (HasAnyTouch)
	{
		float CurrentTime = UGameplayStatics::GetRealTimeSeconds(context);

		if (CurrentTime - LastTouchDown < MaxTapTime)
		{
			if (TapCount >= 3 && !HasConstTap)
			{
				HasConstTap = true;
				for (IGEGestureHandlerDelegate* _delegate : delegates)
				{
					_delegate->ConstTapStart(x, y);
				}
			}
		}

		if (CurrentTime - LastTouchDown > MaxTapTime && HasTouchDown)
		{
			if (TapCount == 0) // A Swip In Happening or has started
			{
				for (IGEGestureHandlerDelegate* _delegate : delegates)
				{
					if (!SwipeStarted)
					{
						_delegate->SwipeStart(x, y);
						SwipeStarted = true;
					}
					else
					{
						_delegate->SwipeUpdate(x, y);
					}
				}
			}
		}

		if (CurrentTime - LastTouchUp > MaxTapTime && !HasTouchDown)
		{
			UE_LOG(LogTemp, Warning, TEXT("Time > MaxTapTime TouchCount: %i"), TapCount);
			
			if (SwipeStarted)
			{
				SwipeStarted = false;
				for (IGEGestureHandlerDelegate* _delegate : delegates)
				{
					_delegate->SwipeEnd(x, y);
				}
			}

			if (HasConstTap)
			{
				HasConstTap = false;
				for (IGEGestureHandlerDelegate* _delegate : delegates)
				{
					_delegate->ConstTapEnd(x, y);
				}
				TapCount = 0;
			}

			if (TapCount > 0)
			{
				if (TapCount == 1)
				{
					for (IGEGestureHandlerDelegate* _delegate : delegates)
					{
						_delegate->SingleTap(x, y);
					}
				}
				else if (TapCount == 2)
				{
					for (IGEGestureHandlerDelegate* _delegate : delegates)
					{
						_delegate->DoubleTap(x, y);
					}
				}
			}

			TapCount = 0;
			HasAnyTouch = false;
		}
	}
}

void UGEGestureHandler::RegisterDelegate(IGEGestureHandlerDelegate * _delegate)
{
	FScopeLock lock(&mutex);
	delegates.Add(_delegate);
}

void UGEGestureHandler::UnregisterDelegate(IGEGestureHandlerDelegate * _delegate)
{
	FScopeLock lock(&mutex);
	delegates.Remove(_delegate);
}

void UGEGestureHandler::TouchDown(float x, float y)
{
	FScopeLock lock(&mutex);
	LastTouchDown = UGameplayStatics::GetRealTimeSeconds(context);
	HasTouchDown = true;
	LOG("TouchDown");
	HasAnyTouch = true;
}

void UGEGestureHandler::TouchUp(float x, float y)
{
	FScopeLock lock(&mutex);
	LastTouchUp = UGameplayStatics::GetRealTimeSeconds(context);

	if (LastTouchUp - LastTouchDown <= TimeToTap)
	{
		TappOccured(x,y);
	}

	HasTouchDown = false;
	UE_LOG(LogTemp,Warning, TEXT("TouchUp TimeDiff %f"), LastTouchUp - LastTouchDown);
}

void UGEGestureHandler::TappOccured(float x,float y)
{
	TapCount++;

	LOG("TappOccured");
}