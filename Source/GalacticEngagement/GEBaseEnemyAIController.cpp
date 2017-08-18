// Fill out your copyright notice in the Description page of Project Settings.

#include "GEBaseEnemyAIController.h"
#include "GEBaseShip.h"

void AGEBaseEnemyAIController::Tick(float DeltaTime)
{
	//if (GEngine)GEngine->AddOnScreenDebugMessage(1, 0.2f, FColor::Yellow, FString::Printf(TEXT("Possesed Pawn Tick !")));

	if (ControlledShip)
	{
		if (AActor* Target = ControlledShip->GetTarget())
		{
			if (ControlledShip->ShouldFireGun())
			{
				if (ControlledShip->ShouldFireMainGun())
				{
					ControlledShip->SelectedGun = ESelectedGun::SG_Main;
					ControlledShip->FireGunDownMapping();
				}
				else if (ControlledShip->ShouldFireSecondaryGun())
				{
					ControlledShip->SelectedGun = ESelectedGun::SG_Secondary;
					ControlledShip->FireGunDownMapping();
				}
				else
				{
					ControlledShip->FireGunReleaseMapping();
				}
			}
			else
			{
				ControlledShip->FireGunReleaseMapping();
				ControlledShip->MoveTo(Target);
			}
		}
		else
		{
			ControlledShip->FireGunReleaseMapping();
			ControlledShip->SearchForTarget(1000);
		}
		
	}

}

void AGEBaseEnemyAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);
	ControlledShip = Cast<AGEBaseShip>(InPawn);
	//if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Possesed Pawn !")));
}

void AGEBaseEnemyAIController::UnPossess()
{
	Super::UnPossess();
	ControlledShip = 0;
}