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
			actor->SetActorLocation(Origin + (direction * FMath::Sqrt(WorldRadiusToScreenEdgeSqr)));
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

			float X = FMath::RandRange(-3000, 3000);
			float Y = FMath::RandRange(-3000, 3000);

			transform.SetLocation(FVector(X, Y, FarPlane));
			AActor * actor = world->SpawnActor(PlaneAcotorClass, &transform);
			ActorsInBackground.Add(actor);
		}
	}
}