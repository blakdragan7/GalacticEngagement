// Fill out your copyright notice in the Description page of Project Settings.

#include "GEThrusterBaseComponent.h"
#include "GEBaseShip.h"
#include "GEGameStatistics.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

UGEThrusterBaseComponent::UGEThrusterBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	IsAccelerating = false;
	NeedsDirectionUpdate = false;
	HitLastFrame = false;
	MaxSpeed = 500;
	MaxAccel = 250;
	MaxRotationRate = 180;
	MaxRotationAccel = 90;
	CurrentRotationRate = 0;
	RotationDeccel = 90;
	SpeedDeccel = 500;

}

void UGEThrusterBaseComponent::SetControlledShip(AGEBaseShip * ship)
{
	check(ship);
	WorldMoveToLocation = ship->GetActorLocation();
	controlledShip = ship;
}

const FVector UGEThrusterBaseComponent::GetMoveToLocation() const
{
	return WorldMoveToLocation;
}

void UGEThrusterBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	if (controlledShip)
	{
		if (IsValid(controlledShip)) // Update Position and rotation if controlledShip is valid
		{
			FVector CurrentLocation = controlledShip->GetCurrentLocation();
			FRotator CurrentRotation = controlledShip->GetCurrentRotation();

			FVector NewLocation = CurrentLocation + (CurrentVelocity * DeltaTime);
			FHitResult hitResult;
			FVector actualLocation = NewLocation;
			if (!controlledShip->SetLocation(NewLocation, true, hitResult))
			{
				FVector Direction = hitResult.Normal;
				Direction.Z = 0;
				Direction.Normalize();
				CurrentVelocity = Direction * CurrentVelocity.Size();
				UE_LOG(LogTemp, Log, TEXT("Hit Something !!"));
			}
			if (NeedsDirectionUpdate)
			{
				const FVector ViewportSize = FVector(GEngine->GameViewport->Viewport->GetSizeXY());
				const FVector ViewportCenter = FVector(ViewportSize.X / 2, ViewportSize.Y / 2, 0);

				DesiredRotation = UKismetMathLibrary::MakeRotFromX(WorldMoveToLocation - ViewportCenter);
				DesiredRotation.Yaw += 90; // Convert To Correct Coord Space
				DesiredRotation.Normalize();
			}
			if (!DesiredRotation.ContainsNaN())
			{
				float MaxDeltaYaw = CurrentRotationRate * DeltaTime;
				float deltaAngle = FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, DesiredRotation.Yaw);
				float currentDelta = 0;
				if (FMath::Abs(MaxDeltaYaw) >= FMath::Abs(deltaAngle))
				{
					CurrentRotation.Yaw = DesiredRotation.Yaw;
				}
				else
				{
					CurrentRotation.Yaw += MaxDeltaYaw * FMath::Sign(deltaAngle);
				}

				controlledShip->SetRotation(CurrentRotation);
			}

			UpdateMovementRates(controlledShip->GetCurrentForwardVector(), DeltaTime);
		}
		else
		{
			controlledShip = 0;
		}
	}
}

void UGEThrusterBaseComponent::UpdateMovementRates(FVector Direction, float DeltaTime)
{
	if (IsAccelerating)
	{
		FVector DesiredVelocity = CurrentVelocity + (Direction* MaxAccel * DeltaTime);
		if (GEGameStatistics::VectorLessThenVector(DesiredVelocity, CurrentVelocity))
		{
			CurrentVelocity = DesiredVelocity;
			//if (GEngine)GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, TEXT("Less Then Current Velocity"));
		}
		else if (GEGameStatistics::VectorLessThenMagnitude(DesiredVelocity, MaxSpeed))
		{
			CurrentVelocity = DesiredVelocity;
			//if(GEngine)GEngine->AddOnScreenDebugMessage(1,1.0f,FColor::Yellow,TEXT("Less Then Max Speed"));
		}
		else
		{
			// Not less then current speed or max speed
			//if (GEngine)GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, TEXT("Not less then current speed or max speed"));
			FVector ProjectedVector = CurrentVelocity.ProjectOnToNormal(Direction) * 0.99;
			CurrentVelocity = FMath::Lerp<FVector, float>(CurrentVelocity, ProjectedVector, DeltaTime);
		}
	}
	if (FMath::Abs(CurrentRotationRate) < MaxRotationRate)
	{
		CurrentRotationRate += MaxRotationAccel * DeltaTime;
		CurrentRotationRate = FMath::Min<float>(CurrentRotationRate, MaxRotationRate);
	}
}

void UGEThrusterBaseComponent::StopMoving()
{
	IsAccelerating = false;
}

void UGEThrusterBaseComponent::MoveTo(FVector WorldMoveToLocation)
{
	this->WorldMoveToLocation = WorldMoveToLocation;
	IsAccelerating = true;
	NeedsDirectionUpdate = true;
}
