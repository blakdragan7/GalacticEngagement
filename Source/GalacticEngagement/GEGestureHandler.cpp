// Fill out your copyright notice in the Description page of Project Settings.

#include "GEGestureHandler.h"

#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

UGEGestureHandler::UGEGestureHandler()
{
	LastTouchDown	= 0;
	LastTouchUp		= 0;
	TapCount		= 0;

	HasTouchDown	= false;
	SwipeStarted	= false;
	HasConstTap		= false;

	TimeToTap		= 0.2f;
	MaxTapTime		= 0.2f;
}

void UGEGestureHandler::Tick(float DeltaTime,float x,float y)
{
	FScopeLock lock(&CRSection);

	float CurrentTime = UGameplayStatics::GetRealTimeSeconds(this);

	if (CurrentTime - LastTouchDown > TimeToTap && HasTouchDown)
	{
		if (TapCount == 0) // A Swip In Happening or has started
		{
			for (PGEGestureHandlerDelegate* _delegate : delegates)
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

	if (CurrentTime - LastTouchUp > TimeToTap && !HasTouchDown)
	{
		if (HasConstTap)
		{
			HasConstTap = false;
			for (PGEGestureHandlerDelegate* _delegate : delegates)
			{
				_delegate->ConstTapEnd(x, y);
			}
			TapCount = 0;
		}

		else if (TapCount > 0)
		{
			if (TapCount == 1)
			{
				for (PGEGestureHandlerDelegate* _delegate : delegates)
				{
					_delegate->SingleTap(x, y);
				}
			}
			else if (TapCount == 2)
			{
				for (PGEGestureHandlerDelegate* _delegate : delegates)
				{
					_delegate->DoubleTap(x, y);
				}
			}

			TapCount = 0;
		}
	}
}

void UGEGestureHandler::RegisterDelegate(PGEGestureHandlerDelegate * _delegate)
{
	FScopeLock lock(&CRSection);
	delegates.Add(_delegate);
}

void UGEGestureHandler::UnregisterDelegate(PGEGestureHandlerDelegate * _delegate)
{
	FScopeLock lock(&CRSection);
	delegates.Remove(_delegate);
}

void UGEGestureHandler::TouchDown(float x, float y)
{
	FScopeLock lock(&CRSection);
	LastTouchDown = UGameplayStatics::GetRealTimeSeconds(this);
	HasTouchDown = true;
}

void UGEGestureHandler::TouchUp(float x, float y)
{
	FScopeLock lock(&CRSection);
	LastTouchUp = UGameplayStatics::GetRealTimeSeconds(this);

	if (LastTouchUp - LastTouchDown <= TimeToTap)
	{
		TappOccured(x,y);
	}
	if (SwipeStarted)
	{
		SwipeStarted = false;
		for (PGEGestureHandlerDelegate* _delegate : delegates)
		{
			_delegate->SwipeEnd(x, y);
		}
	}

	HasTouchDown = false;
}

void UGEGestureHandler::TappOccured(float x,float y)
{
	TapCount++;

	if (TapCount >= 3 && !HasConstTap)
	{
		HasConstTap = true;
		for (PGEGestureHandlerDelegate* _delegate : delegates)
		{
			_delegate->ConstTapStart(x, y);
		}
	}
}