// Fill out your copyright notice in the Description page of Project Settings.

#include "GESphericalTravelManager.h"
#include "GEGameStatistics.h"
#include "GEBaseShip.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

GESphericalTravelManager::GESphericalTravelManager(FVector origin, float sphereRadius)
{
	this->SphereRadius = sphereRadius;
	this->Origin = origin;
}

GESphericalTravelManager::~GESphericalTravelManager()
{
}

void GESphericalTravelManager::ProjectPointToSphere(FVector Point,FVector &ProjectedPoint,FVector& Up)
{
	Up = (Point - Origin).GetSafeNormal();
	ProjectedPoint = Origin + (Up * SphereRadius);
}

bool GESphericalTravelManager::MoveShipToPoint(AGEBaseShip * actor, FVector inDesiredPoint, FVector & outActualPoint, FHitResult & outHitResult)
{
	FVector Up;
	FVector CurrentLocation = actor->GetActorLocation();
	this->ProjectPointToSphere(inDesiredPoint, outActualPoint, Up);
	bool success = actor->SetLocation(outActualPoint, true, outHitResult);
	if (success)
	{
		FRotator rotation = UKismetMathLibrary::MakeRotFromZX(Up, actor->GetActorForwardVector());
		actor->SetActorRotation(rotation);
	}
	return success;
}

bool GESphericalTravelManager::MoveActorToPoint(AActor* actor, FVector inDesiredPoint, FVector& outActualPoint,FHitResult& outHitResult)
{
	FVector Up;
	FVector CurrentLocation = actor->GetActorLocation();
	this->ProjectPointToSphere(inDesiredPoint, outActualPoint, Up);
	bool success = actor->SetActorLocation(outActualPoint, true, &outHitResult);
	if (success)
	{
		FRotator rotation = UKismetMathLibrary::MakeRotFromZX(Up,actor->GetActorForwardVector());
		actor->SetActorRotation(rotation);
	}
	return success;
}

float GESphericalTravelManager::GetDistanceToActor(AActor* actor1, class AActor* actor2)
{
	FVector Ap1 = actor1->GetActorLocation();
	FVector Ap2 = actor2->GetActorLocation();

	FVector A1SphereDirection = (Ap1 - Origin).GetSafeNormal();
	FVector A2SphereDirection = (Ap2 - Origin).GetSafeNormal();

	FVector Cross = FVector::CrossProduct(A1SphereDirection, A2SphereDirection);
	float CrossMag = Cross.Size();
	return FMath::Atan(CrossMag / (FVector::DotProduct(A1SphereDirection,A2SphereDirection))) * SphereRadius;
}

FVector GESphericalTravelManager::GetShortestDirectionTo(AActor* actor1, class AActor* actor2)
{
	FVector Ap1 = actor1->GetActorLocation();
	FVector Ap2 = actor2->GetActorLocation();

	FVector A1SphereDirection = (Ap1 - Origin).GetSafeNormal();

	FVector Ap1P = FVector::VectorPlaneProject(Ap1,A1SphereDirection);
	FVector Ap2P = FVector::VectorPlaneProject(Ap2,A1SphereDirection);
	FVector Direction = (Ap2P - Ap1P).GetSafeNormal();

	return Direction;
}