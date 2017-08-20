// Fill out your copyright notice in the Description page of Project Settings.

#include "GEEngineBaseComponent.h"
#include "GEBaseShip.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
UGEEngineBaseComponent::UGEEngineBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	IsAccelerating = false;
	MaxSpeed = 1000;
	MaxAccel = 200;
	CurrentSpeed = 0;
	MaxRotationRate = 180;
	MaxRotationAccel = 90;
	CurrentRotationRate = 0;
	RotationDeccel = 90;
	SpeedDeccel = 500;
}

void UGEEngineBaseComponent::SetControlledShip(AGEBaseShip * ship)
{
	check(ship);
	WorldMoveToLocation = ship->GetActorLocation();
	controlledShip = ship;
}

const FVector UGEEngineBaseComponent::GetMoveToLocation() const
{
	return WorldMoveToLocation;
}

void UGEEngineBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	if (controlledShip)
	{
		if (IsValid(controlledShip)) // Update Position and rotation if controlledShip is valid
		{
			FVector CurrentLocation = controlledShip->GetActorLocation();
			FRotator CurrentRotation = controlledShip->GetCurrentRotation();
			FRotator FacingRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, WorldMoveToLocation);
			if (!FacingRotation.ContainsNaN())
			{
				float MaxDeltaYaw = CurrentRotationRate * DeltaTime;
				float deltaAngle = FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, FacingRotation.Yaw);
				if (MaxDeltaYaw >= FMath::Abs(deltaAngle))
				{
					CurrentRotation.Yaw = FacingRotation.Yaw;
				}
				else
				{
					CurrentRotation.Yaw += MaxDeltaYaw * FMath::Sign(deltaAngle);
				}

				controlledShip->SetRotation(CurrentRotation);
			}
			FVector ForwardVector = controlledShip->GetCurrentForwardVector();
			FVector NewLocation = CurrentLocation + (ForwardVector * CurrentSpeed * DeltaTime);
			//if(GEngine)GEngine->AddOnScreenDebugMessage(1,15.0f,FColor::Yellow,FString::Printf(TEXT("World Location = %s"),*NewLocation.ToString()));
			if (!controlledShip->SetActorLocation(NewLocation, true))
			{
				UE_LOG(LogTemp, Log,TEXT("Hit Something !!"));
			}
			UpdateMovementRates(DeltaTime);
		}
		else
		{
			controlledShip = 0;
		}
	}
}

void UGEEngineBaseComponent::UpdateMovementRates(float DeltaTime)
{
	if (IsAccelerating)
	{
		if (CurrentSpeed < MaxSpeed)
		{
			CurrentSpeed += MaxAccel * DeltaTime;
			CurrentSpeed = FMath::Min<float>(CurrentSpeed, MaxSpeed);
		}
		if (CurrentRotationRate < MaxRotationRate)
		{
			CurrentRotationRate += MaxRotationAccel * DeltaTime;
			CurrentRotationRate = FMath::Min<float>(CurrentRotationRate, MaxRotationRate);
		}
	}
	else
	{
		if (CurrentSpeed > 0)
		{
			CurrentSpeed -= SpeedDeccel * DeltaTime;
			CurrentSpeed = FMath::Max<float>(CurrentSpeed, 0);
		}
		if (CurrentRotationRate > 0)
		{
			CurrentRotationRate -= RotationDeccel * DeltaTime;
			CurrentRotationRate = FMath::Max<float>(CurrentRotationRate, 0);
		}
	}
}

void UGEEngineBaseComponent::StopMoving()
{
	IsAccelerating = false;
}

void UGEEngineBaseComponent::MoveTo(FVector WorldMoveToLocation)
{
	this->WorldMoveToLocation = WorldMoveToLocation;
	IsAccelerating = true;
}
