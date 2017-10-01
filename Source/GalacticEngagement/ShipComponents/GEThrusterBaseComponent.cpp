// Fill out your copyright notice in the Description page of Project Settings.

#include "GEThrusterBaseComponent.h"
#include "GEBaseShip.h"
#include "Interfaces/GEVelocityEffector.h"
#include "Math/GEGameStatistics.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine.h"

void UGEThrusterBaseComponent::Thrusting(float percentage)
{
	if (IsValid(ControlledShip))
		ControlledShip->Thrusting(percentage);
}

UGEThrusterBaseComponent::UGEThrusterBaseComponent()
{
	IsAccelerating = false;
	ComponentType = EShipComponentType::SC_Thruster;
	NeedsScreenDirectionUpdate = false;
	NeedsWorldDirectionUpdate = false;
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
	ControlledShip = ship;
}

const FVector UGEThrusterBaseComponent::GetMoveToLocation() const
{
	return WorldMoveToLocation;
}

bool UGEThrusterBaseComponent::Server_StopMoving_Validate()
{
	return true;
}

void UGEThrusterBaseComponent::Server_StopMoving_Implementation()
{
	IsAccelerating = false;
}

void UGEThrusterBaseComponent::TickComponent(float DeltaTime)
{
	if ((ControlledShip && ControlledShip->Role == ROLE_Authority) || ControlledShip->bIsMultiplayer == false)
	{
		if (IsValid(ControlledShip)) // Update Position and rotation if controlledShip is valid
		{
			FVector CurrentLocation = ControlledShip->GetCurrentLocation();
			FRotator CurrentRotation = ControlledShip->GetCurrentRotation();

			FVector NewLocation = CurrentLocation + (CurrentVelocity * DeltaTime);
			FHitResult hitResult;
			FVector actualLocation = NewLocation;
			if (!ControlledShip->SetLocation(NewLocation, true, hitResult))
			{
				FVector Direction = hitResult.Normal;
				Direction.Z = 0;
				Direction.Normalize();
				CurrentVelocity = Direction * CurrentVelocity.Size();
				UE_LOG(LogTemp, Log, TEXT("Hit Something !!"));
			}
			if (NeedsScreenDirectionUpdate)
			{
				const FVector2D ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();
				const FVector2D ViewportCenter = ViewportSize / 2;

				FVector X = FVector(ScreenMoveToLocation - ViewportCenter,0);

				DesiredRotation = UKismetMathLibrary::MakeRotFromX(X);
				DesiredRotation.Yaw += 90; // Convert To Correct Coord Space
				DesiredRotation.Normalize();
			}
			else if (NeedsWorldDirectionUpdate)
			{
				DesiredRotation = UKismetMathLibrary::MakeRotFromX(WorldMoveToLocation - CurrentLocation);
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

				ControlledShip->SetRotation(CurrentRotation);
			}

			UpdateMovementRates(ControlledShip->GetCurrentForwardVector(), DeltaTime);
			UpdateVelocityFromEffectors(actualLocation,DeltaTime);
		}
		else
		{
			ControlledShip = 0;
		}
	}
}

void UGEThrusterBaseComponent::UpdateVelocityFromEffectors(FVector CurrentPosition,float DeltaTime)
{
	for (AActor* effector : Effectors)
	{
		CurrentVelocity += IGEVelocityEffector::Execute_GetVelocityDeltaForPosition(effector,CurrentPosition) * DeltaTime;
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
			FVector ProjectedVector = CurrentVelocity.ProjectOnToNormal(Direction);
			CurrentVelocity = FMath::Lerp<FVector, float>(CurrentVelocity, ProjectedVector, DeltaTime);
		}
	}
	if (FMath::Abs(CurrentRotationRate) < MaxRotationRate)
	{
		CurrentRotationRate += MaxRotationAccel * DeltaTime;
		CurrentRotationRate = FMath::Min<float>(CurrentRotationRate, MaxRotationRate);
	}

	Thrusting(GEGameStatistics::VectorPercentageOfMag(CurrentVelocity,MaxSpeed));
}

void UGEThrusterBaseComponent::StopMoving()
{
	Server_StopMoving();
}

void UGEThrusterBaseComponent::MoveTo(FVector2D screenMoveToLocation)
{
	this->ScreenMoveToLocation = screenMoveToLocation;
	IsAccelerating = true;
	NeedsScreenDirectionUpdate = true;
}

void UGEThrusterBaseComponent::MoveTo(FVector worldMoveToLocation)
{
	this->WorldMoveToLocation = worldMoveToLocation;
	IsAccelerating = true;
	NeedsWorldDirectionUpdate = true;
}

bool UGEThrusterBaseComponent::AddEffector(AActor * actor)
{
	if (actor->GetClass()->ImplementsInterface(UGEVelocityEffector::StaticClass()))
	{
		Effectors.AddUnique(actor);
		return true;
	}
	return false;
}

bool UGEThrusterBaseComponent::RemoveEffector(AActor * actor)
{
	Effectors.Remove(actor);
	return true;
}
