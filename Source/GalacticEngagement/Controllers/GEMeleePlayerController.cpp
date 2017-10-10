// Fill out your copyright notice in the Description page of Project Settings.

#include "GEMeleePlayerController.h"
#include "GEBaseShip.h"
#include "Runtime/Engine/Public/ConvexVolume.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Camera/CameraActor.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Engine/LocalPlayer.h"
#include "Runtime/Engine/Public/SceneView.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Modes/GEPlayerMelleeGameMode.h"
#include "Math/BackgroundPlane.h"
#include "Engine.h"

AGEMeleePlayerController::AGEMeleePlayerController()
{
	bShowMouseCursor = true;
	BeganStarField = false;
	HasSpawnedShip = false;
}

void AGEMeleePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdatePlanes();
}

void AGEMeleePlayerController::BeginPlay()
{
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AGEMeleePlayerController::LoadCustomShipData, 1.0, false);
}

void AGEMeleePlayerController::OnConstruction(const FTransform & Transform)
{
	if (IsLocalController())
	{
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
			FBackgroundPlane *plane = new FBackgroundPlane;

			plane->SetActorClass(BackgroundClasses[i]);
			plane->SetPlaneNormal(FVector(0, 0, 1));
			plane->SetActorDensity(BackgroundLayerDensity[i]);
			plane->SetFarPlane(LayerZPlanes[i]);

			if (BackgroundPlanes.Num() > i)
			{
				delete BackgroundPlanes[i];
				BackgroundPlanes[i] = plane;
			}
			else
				BackgroundPlanes.Add(plane);
		}
	}
}

void AGEMeleePlayerController::BeginDestroy()
{
	Super::BeginDestroy();
	for (auto plane : BackgroundPlanes)
	{
		delete plane;
	}
	BackgroundPlanes.Empty();
}

void AGEMeleePlayerController::ClientBeginStarField_Implementation()
{
	BeganStarField = true;

	if (AGEBaseShip* ship = Cast<AGEBaseShip>(GetPawn()))
	{
		for (int32 i = 0; i < BackgroundPlanes.Num(); i++)
		{
			FBackgroundPlane* plane = BackgroundPlanes[i];
			plane->SetRadiusToEdggeSqr(FMath::Square<int32>(ship->MaxCameraArmLength*1.2));
			plane->SpawnAllActorsForBackground(GetWorld());
		}
	}
}

void AGEMeleePlayerController::UpdatePlanes()
{
	if (IsLocalController() && BeganStarField)
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
				FBackgroundPlane* plane = BackgroundPlanes[i];

				float farplane = plane->GetFarPlane();
				float linedistance = FMath::Abs(LowerLeft.Z - farplane);

				FVector Origin = FMath::LinePlaneIntersection(Center, Center + (CenterDirection * linedistance), FVector(0, 0, farplane), FVector(0, 0, 1));

				plane->SetOrigin(Origin);

				plane->UpdateActors();
			}
		}
	}
}


void AGEMeleePlayerController::LoadCustomShipData()
{
	if (USaveGame* saveGame = UGameplayStatics::LoadGameFromSlot("CustomShip", 0))
	{
		if (UCustomShipSave* shipSave = Cast<UCustomShipSave>(saveGame))
		{
			FNetComponentSaveStruct netStruct;

			netStruct.EngineClass = shipSave->EngineClass;
			netStruct.ThrusterClass = shipSave->ThrusterClass;
			netStruct.ShipClass = shipSave->ShipClass;

			netStruct.PrimaryGunClasses = shipSave->PrimaryGunClasses;
			netStruct.SecondayGunClasses = shipSave->SecondayGunClasses;

			RequestSpawnShip(netStruct);
		}
	}
}

bool AGEMeleePlayerController::RequestSpawnShip_Validate(struct FNetComponentSaveStruct ShipSave)
{
	return HasSpawnedShip == false;
}

void AGEMeleePlayerController::RequestSpawnShip_Implementation(struct FNetComponentSaveStruct ShipSave)
{
	if (AGEPlayerMelleeGameMode* gameMode = Cast<AGEPlayerMelleeGameMode>(GetWorld()->GetAuthGameMode()))
	{
		HasSpawnedShip = true;
		gameMode->SpawnPlayer(this, ShipSave);
	}
}