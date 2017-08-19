// Fill out your copyright notice in the Description page of Project Settings.

#include "GEEngineBaseComponent.h"

UGEEngineBaseComponent::UGEEngineBaseComponent()
{
	MaxSpeed = 1000;
	MaxAccel = 200;
	CurrentSpeed = 0;
	MaxRotationRate = 180;
	MaxRotationAccel = 90;
	CurrentRotationRate = 0;
	RotationDeccel = 90;
	SpeedDeccel = 500;
}

void UGEEngineBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
}

void UGEEngineBaseComponent::UpdateMovementRates(float DeltaTime)
{
	if (IsMoving)
	{
		if (CurrentSpeed < MaxSpeed)
		{
			CurrentSpeed += MaxAccel * DeltaTime;
			CurrentSpeed = FMath::Clamp<float>(CurrentSpeed, 0, MaxSpeed);
		}
		if (CurrentRotationRate < MaxRotationRate)
		{
			CurrentRotationRate += MaxRotationAccel * DeltaTime;
			CurrentRotationRate = FMath::Clamp<float>(CurrentRotationRate, 0, MaxRotationRate);
		}
	}
	else
	{
		if (CurrentSpeed > 0)
		{
			CurrentSpeed -= SpeedDeccel * DeltaTime;
			CurrentSpeed = FMath::Clamp<float>(CurrentSpeed, 0, MaxSpeed);
		}
		if (CurrentRotationRate > 0)
		{
			CurrentRotationRate -= RotationDeccel * DeltaTime;
			CurrentRotationRate = FMath::Clamp<float>(CurrentRotationRate, 0, MaxRotationRate);
		}
	}
}

void UGEEngineBaseComponent::StopMoving()
{
	IsMoving = false;
}

void UGEEngineBaseComponent::MoveTo(FVector WorldMoveToLocation)
{
	IsMoving = true;
}
