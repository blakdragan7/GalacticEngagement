// Fill out your copyright notice in the Description page of Project Settings.

#include "GEMeleePlayerController.h"
#include "Runtime/Engine/Public/ConvexVolume.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Camera/CameraActor.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Engine/LocalPlayer.h"
#include "Runtime/Engine/Public/SceneView.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"

AGEMeleePlayerController::AGEMeleePlayerController()
{
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Scene Collision Box"));
	CollisionBox->bHiddenInGame = false;
	CollisionBox->SetVisibility(true,true);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AGEMeleePlayerController::OnComponentOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &AGEMeleePlayerController::OnComponentOverlapEnd);

	bShowMouseCursor = true;
}

void AGEMeleePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (AActor* viewTarget = GetViewTarget())
	{
		AscertainCameraBounds();
		CollisionBox->SetWorldLocation(viewTarget->GetActorLocation(),true);
	}
}

void AGEMeleePlayerController::BeginPlay()
{
	SpawnAllStars();
}

void AGEMeleePlayerController::OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->StaticClass() == StarClass)
	{

	}
}

void AGEMeleePlayerController::OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->StaticClass() == StarClass)
	{
		UE_LOG(LogTemp,Warning, TEXT("Leaving %s"),*OtherActor->GetName());
	}
}

void AGEMeleePlayerController::SpawnAllStars()
{
	if (StarClass)
	{
		for (int32 i = 0; i < StarDensity; i++)
		{
			FTransform transform;

			float X = FMath::RandRange(-3000, 3000);
			float Y = FMath::RandRange(-3000,3000);

			transform.SetLocation(FVector(X, Y, StarZPlane));
			GetWorld()->SpawnActor(StarClass, &transform);
		}
	}
}

void AGEMeleePlayerController::AscertainCameraBounds()
{
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
	{
		const FVector2D ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();

		FVector LowerLeft, LowerLeftDirection;
		DeprojectScreenPositionToWorld(0, 0, LowerLeft, LowerLeftDirection);

		FVector UpperRight, UpperRightDirection;
		DeprojectScreenPositionToWorld(ViewportSize.X, ViewportSize.Y, UpperRight, UpperRightDirection);

		FVector BottomLeft = FMath::LinePlaneIntersection(LowerLeft, LowerLeft + (LowerLeftDirection * 1000), FVector(0, 0, StarZPlane), FVector(0, 0, 1));
		FVector TopRight = FMath::LinePlaneIntersection(UpperRight, UpperRight + (UpperRightDirection * 1000), FVector(0, 0, StarZPlane), FVector(0, 0, 1));

		CameraBounds = ((TopRight - BottomLeft) / 2.0f).GetAbs();
		CameraBounds.Z = 1000;
		CollisionBox->SetBoxExtent(CameraBounds, false);
		UE_LOG(LogTemp,Warning,TEXT("New Extents %s"),*CameraBounds.ToString());
	}
}