// Fill out your copyright notice in the Description page of Project Settings.

#include "BackgroundPlane.h"
#include "Runtime/Engine/Classes/Engine/World.h"

FBackgroundPlane::FBackgroundPlane()
{
	FarPlane = 0;
	WorldRadiusToScreenEdgeSqr = 0;
	PlaneNormal = FVector(0,0,1);
	ActorDensity = 0;
}

FBackgroundPlane::~FBackgroundPlane()
{	
	DestroyAllActors();
}

void FBackgroundPlane::DestroyAllActors()
{
	for (AActor *actor : ActorsInBackground)
	{
		actor->Destroy();
	}

	ActorsInBackground.Empty();
}

void FBackgroundPlane::UpdateActors()
{
	for (AActor *actor : ActorsInBackground)
	{
		FVector location = actor->GetActorLocation();
		FVector distance = (location - Origin);
		if (distance.SizeSquared() > WorldRadiusToScreenEdgeSqr)
		{
			FVector direction = -distance.GetSafeNormal();
			actor->SetActorLocation(Origin + (direction * WorldRadiusToScreenEdge));
		}
	}
}

void FBackgroundPlane::SpawnAllActorsForBackground(UWorld* world)
{
	DestroyAllActors();

	if (PlaneAcotorClass)
	{
		for (int32 i = 0; i < ActorDensity; i++)
		{
			FTransform transform;

			float X = FMath::RandRange(-WorldRadiusToScreenEdge, WorldRadiusToScreenEdge);
			float Y = FMath::RandRange(-WorldRadiusToScreenEdge, WorldRadiusToScreenEdge);

			transform.SetLocation(FVector(X, Y, FarPlane));
			if (AActor * actor = world->SpawnActor(PlaneAcotorClass, &transform))
			{
				ActorsInBackground.Add(actor);
			}
			else
			{
				UE_LOG(LogTemp,Warning,TEXT("Could Not Spawn Actor At Position X: %f, Y: %f, Z: %f") , X,Y,FarPlane);
			}
		}
	}
}