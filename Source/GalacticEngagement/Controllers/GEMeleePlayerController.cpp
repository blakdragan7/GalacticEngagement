// Fill out your copyright notice in the Description page of Project Settings.

#include "GEMeleePlayerController.h"
#include "Runtime/Engine/Public/ConvexVolume.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Camera/CameraActor.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Engine/LocalPlayer.h"
#include "Runtime/Engine/Public/SceneView.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Math/BackgroundPlane.h"

AGEMeleePlayerController::AGEMeleePlayerController()
{
	bShowMouseCursor = true;
}

void AGEMeleePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdatePlanes();
}

void AGEMeleePlayerController::BeginPlay()
{
	if (IsLocalController())
	{
		for (FBackgroundPlane &plane : BackgroundPlanes)
		{
			plane.SpawnAllActorsForBackground(GetWorld());
		}
	}
}

void AGEMeleePlayerController::OnConstruction(const FTransform & Transform)
{
	if (IsLocalController())
	{
		BackgroundPlanes.Empty();

		int32 actorclasssize = BackgroundClasses.Num();
		if (actorclasssize != BackgroundLayerDensity.Num())
		{
			BackgroundLayerDensity.SetNum(actorclasssize, true);
		}
		if (actorclasssize != LayerZPlanes.Num())
		{
			LayerZPlanes.SetNum(actorclasssize, true);
		}
		if (actorclasssize != BackgroundPlanes.Num())
		{
			BackgroundPlanes.SetNum(actorclasssize, true);
		}

		for (int32 i = 0; i < actorclasssize; i++)
		{
			FBackgroundPlane plane;

			plane.SetActorClass(BackgroundClasses[i]);
			plane.SetPlaneNormal(FVector(0, 0, 1));
			plane.SetActorDensity(BackgroundLayerDensity[i]);
			plane.SetFarPlane(LayerZPlanes[i]);

			BackgroundPlanes.Add(plane);
		}
	}
}

void AGEMeleePlayerController::UpdatePlanes()
{
	if (IsLocalController())
	{
		if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
		{
			const FVector2D ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();

			FVector LowerLeft, LowerLeftDirection;
			DeprojectScreenPositionToWorld(0, 0, LowerLeft, LowerLeftDirection);

			FVector Center, CenterDirection;
			DeprojectScreenPositionToWorld(ViewportSize.X / 2, ViewportSize.Y / 2, Center, CenterDirection);

			for (int32 i = 0; i < BackgroundPlanes.Num(); i++)
			{
				FBackgroundPlane& plane = BackgroundPlanes[i];

				float farplane = plane.GetFarPlane();
				float linedistance = FMath::Abs(LowerLeft.Z - farplane);

				FVector BottomLeft = FMath::LinePlaneIntersection(LowerLeft, LowerLeft + (LowerLeftDirection * linedistance), FVector(0, 0, farplane), FVector(0, 0, 1));
				FVector Origin = FMath::LinePlaneIntersection(Center, Center + (CenterDirection * linedistance), FVector(0, 0, farplane), FVector(0, 0, 1));

				FVector Distance = (Origin - BottomLeft).GetAbs();

				plane.SetOrigin(Origin);
				plane.SetRadiusToEdggeSqr(Distance.SizeSquared2D());

				plane.UpdateActors();
			}
		}
	}
}
