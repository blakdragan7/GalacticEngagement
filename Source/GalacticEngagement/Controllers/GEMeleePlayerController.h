// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GEMeleePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GALACTICENGAGEMENT_API AGEMeleePlayerController : public APlayerController
{
	GENERATED_BODY()
private:
	FVector CameraBounds;
	// Collision box used for determining if an object should be wraped
	UPROPERTY(Category = Background, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Environment)
	TSubclassOf<AActor> StarClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
	int32 StarDensity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
	float StarZPlane;

private:
	void SpawnAllStars();
	void AscertainCameraBounds();

public:
	AGEMeleePlayerController();
	virtual void Tick(float DeltaTime) override;
	void BeginPlay()override;

	void OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	void OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
