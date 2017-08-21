// Fill out your copyright notice in the Description page of Project Settings.

#include "GEEngineBaseComponent.h"
#include "GEBaseShip.h"
#include "GEGameStatistics.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
UGEEngineBaseComponent::UGEEngineBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	IsAccelerating = false;
	NeedsDirectionUpdate = false;
	MaxSpeed = 500;
	MaxAccel = 250;
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
			if (NeedsDirectionUpdate)
			{
				DesiredRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, WorldMoveToLocation);
				NeedsDirectionUpdate = false;
			}
			if (!DesiredRotation.ContainsNaN())
			{
				float MaxDeltaYaw = CurrentRotationRate * DeltaTime;
				float deltaAngle = FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, DesiredRotation.Yaw);
				if (MaxDeltaYaw >= FMath::Abs(deltaAngle))
				{
					CurrentRotation.Yaw = DesiredRotation.Yaw;
				}
				else
				{
					CurrentRotation.Yaw += MaxDeltaYaw * FMath::Sign(deltaAngle);
				}

				controlledShip->SetRotation(CurrentRotation);
			}
			FVector NewLocation = CurrentLocation + (CurrentVelocity * DeltaTime);
			//if(GEngine)GEngine->AddOnScreenDebugMessage(1,15.0f,FColor::Yellow,FString::Printf(TEXT("World Location = %s"),*NewLocation.ToString()));
			if (!controlledShip->SetActorLocation(NewLocation, true))
			{
				UE_LOG(LogTemp, Log,TEXT("Hit Something !!"));
			}
			UpdateMovementRates(controlledShip->GetCurrentForwardVector() ,DeltaTime);
		}
		else
		{
			controlledShip = 0;
		}
	}
}

void UGEEngineBaseComponent::UpdateMovementRates(FVector Direction,float DeltaTime)
{
	if (IsAccelerating)
	{	
		FVector DesiredVelocity = CurrentVelocity + (Direction* MaxAccel * DeltaTime);
		if (GEGameStatistics::VectorLessThenVector(DesiredVelocity,CurrentVelocity))
		{
			CurrentVelocity = DesiredVelocity;
			if (GEngine)GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, TEXT("Less Then Current Velocity"));
		}
		else if (GEGameStatistics::VectorLessThenMagnitude(DesiredVelocity,MaxSpeed))
		{
			CurrentVelocity = DesiredVelocity;
			if(GEngine)GEngine->AddOnScreenDebugMessage(1,1.0f,FColor::Yellow,TEXT("Less Then Max Speed"));
		}
		else
		{
			// Not less then current speed or max speed
			if (GEngine)GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, TEXT("Not less then current speed or max speed"));
			CurrentVelocity = CurrentVelocity.ProjectOnToNormal(Direction);
		}
	}
	if (FMath::Abs(CurrentRotationRate) < MaxRotationRate)
	{
		CurrentRotationRate += MaxRotationAccel * DeltaTime;
		CurrentRotationRate = FMath::Min<float>(CurrentRotationRate, MaxRotationRate);
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
	NeedsDirectionUpdate = true;
}
