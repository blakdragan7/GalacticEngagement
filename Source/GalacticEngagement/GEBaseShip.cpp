// Fill out your copyright notice in the Description page of Project Settings.

#include "GEBaseShip.h"
//#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GEGunBaseComponent.h"
#include "DrawDebugHelpers.h"
#include "GEEngineBaseComponent.h"
#include "GEThrusterBaseComponent.h"
#include "GEGameStatistics.h"
#include "Runtime/Engine/Classes/Components/ArrowComponent.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "GEDamageIndicator.h"
#include "EngineUtils.h"
// Sets default values
AGEBaseShip::AGEBaseShip()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorld;

	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ShipRoot"));
	
	ShipBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipBody"));
	ShipBody->SetupAttachment(RootComponent);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetWorldRotation(FRotator(-90,0,0));
	CameraBoom->TargetArmLength = 600.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller
	CameraBoom->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	
	Engine = CreateDefaultSubobject<UGEEngineBaseComponent>(TEXT("Engine"));
	Engine->SetupAttachment(ShipBody);

	FrontFacingArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("FrontFacingArrow"));
	FrontFacingArrow->SetupAttachment(ShipBody);
	FrontFacingArrow->bHiddenInGame = false;

	MainGun = CreateDefaultSubobject<UGEGunBaseComponent>(TEXT("MainGun"));
	MainGun->SetupAttachment(ShipBody);
	MainGun->SetWorldRotation(FrontFacingArrow->GetComponentRotation());

	SecondaryGun = CreateDefaultSubobject<UGEGunBaseComponent>(TEXT("SecondaryGun"));
	SecondaryGun->SetupAttachment(ShipBody);
	SecondaryGun->SetWorldRotation(FrontFacingArrow->GetComponentRotation());

	Thrusters = CreateDefaultSubobject<UGEThrusterBaseComponent>(TEXT("Thrusters"));
	Thrusters->SetupAttachment(ShipBody);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshCube(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (StaticMeshCube.Object)ShipBody->SetStaticMesh(StaticMeshCube.Object);

	GroundZ = 0;
	MaxSpeed = 1000;
	MaxAccel = 200;
	CurrentSpeed = 0;
	MaxRotationRate = 180;
	MaxRotationAccel = 90;
	CurrentRotationRate = 0;
	RotationDeccel = 90;
	SpeedDeccel = 500;

	CameraOffsetScale = 2;

	AttackAngle = 45.0;
	AttackDistance = 1000.0;
	SlowDownDistance = 600.0;

	MaxHealth = 100;
	CurrentHealth = 0;

	CurrentlyTargetedShip = 0;
	CurrentlyTargetingMe = 0;

	SelectedGun = ESelectedGun::SG_Main;
}

// Called when the game starts or when spawned
void AGEBaseShip::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	FVector CurrentLocation = GetActorLocation();
	MoveToPoint = FVector2D(CurrentLocation.X, CurrentLocation.Y);
}

// Called every frame
void AGEBaseShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(this))
	{
		FVector CurrentLocation = GetActorLocation();
		FVector WorldMoveToLocation;

		if (CurrentlyTargetedShip)
		{
			if (IsValid(CurrentlyTargetedShip))
			{
				WorldMoveToLocation = FVector(MoveToPoint.X, MoveToPoint.Y, 0);
			}
			else
			{
				CurrentlyTargetedShip = 0;
				MoveToPoint = FVector2D(CurrentLocation.X, CurrentLocation.Y);
			}
		}
		else if (APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			FVector WorldLocation;
			FVector WorldDirection;
			if (UGameplayStatics::DeprojectScreenToWorld(controller, MoveToPoint, WorldLocation, WorldDirection))
			{
				FVector EndLocation = WorldLocation - (WorldDirection * (WorldLocation.Z - GroundZ));
				WorldMoveToLocation = FMath::LinePlaneIntersection(WorldLocation, EndLocation, FVector(0, 0, GroundZ), FVector(0, 0, 1));
				//if(GEngine)GEngine->AddOnScreenDebugMessage(1,15.0f,FColor::Yellow,FString::Printf(TEXT("World Location = %s"),*WorldMoveToLocation.ToString()));

				
			}
			else
			{
				// Failed to convert to world position
			}
			// Update Camera if we have a player controller attached
			UpdateCameraPosition();
		}
		{ // Update Position and rotation
			WorldMoveToLocation.Z = CurrentLocation.Z;
			FRotator CurrentRotation = ShipBody->GetComponentRotation();
			FRotator FacingRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, WorldMoveToLocation);
			if (!FacingRotation.ContainsNaN())
			{
				float MaxDeltaYaw = CurrentRotationRate * DeltaTime;
				float deltaAngle = FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, FacingRotation.Yaw);
				if (MaxDeltaYaw >= FMath::Abs(deltaAngle))
				{
					CurrentRotation.Yaw = FacingRotation.Yaw;
				}
				else
				{
					CurrentRotation.Yaw += MaxDeltaYaw * FMath::Sign(deltaAngle);
				}

				ShipBody->SetWorldRotation(CurrentRotation);
			}
			FVector ForwardVector = ShipBody->GetForwardVector();

			SetActorLocation(CurrentLocation + (ForwardVector * CurrentSpeed * DeltaTime));
		}

		UpdateMovementRates(DeltaTime);
		if (FireGunToggle)FireSelectedGun(); // Up to Individual Guns to Limit Fire rate

		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + (ShipBody->GetForwardVector() * 100.0),
			100.f, FColor::Red, false, -1.f, (uint8)'\000', 10.f);
	}
}

// Called to bind functionality to input
void AGEBaseShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAxis("MoveTo", this, &AGEBaseShip::MoveTo);
	InputComponent->BindAction("MainFire", IE_Released, this, &AGEBaseShip::FireGunReleaseMapping);
	InputComponent->BindAction("MainFire", IE_Pressed, this, &AGEBaseShip::FireGunDownMapping);
}

void AGEBaseShip::ReceiveDamage(int32 Damage, FVector DamageLocation)
{
	CurrentHealth -= Damage;
	AGEDamageIndicator::SpawnIndicatorWithDamageAndDuration(this, DamageLocation, 0.25, FString::FromInt(Damage));
	if (CurrentHealth <= 0)
	{
		ShipDestroyed();
		OnShipDeath();
	}
}

int32 AGEBaseShip::GetHealth()
{
	return CurrentHealth;
}

void AGEBaseShip::UpdateCameraPosition()
{
	if (CurrentlyTargetingMe)
	{
		if (IsValid(CurrentlyTargetingMe))
		{
			float distance = GetDistanceTo(CurrentlyTargetingMe) * CameraOffsetScale;
			distance = FMath::Max<float>(distance, 600.0);
			CameraBoom->TargetArmLength = distance;
		}
		else
		{
			CurrentlyTargetingMe = 0;
		}
	}
	else
	{
		CameraBoom->TargetArmLength = 600.0;
	}
}

void AGEBaseShip::UpdateMovementRates(float DeltaTime)
{
	if (IsMoving)
	{
		if (CurrentSpeed < MaxSpeed)
		{
			CurrentSpeed += MaxAccel * DeltaTime;
			CurrentSpeed = FMath::Clamp<float>(CurrentSpeed, 0, MaxSpeed);
		}
		if (CurrentRotationRate < MaxRotationRate)
		{
			CurrentRotationRate += MaxRotationAccel * DeltaTime;
			CurrentRotationRate = FMath::Clamp<float>(CurrentRotationRate, 0, MaxRotationRate);
		}
	}
	else
	{
		if (CurrentSpeed > 0)
		{
			CurrentSpeed -= SpeedDeccel * DeltaTime;
			CurrentSpeed = FMath::Clamp<float>(CurrentSpeed,0,MaxSpeed);
		}
		if (CurrentRotationRate > 0)
		{
			CurrentRotationRate -= RotationDeccel * DeltaTime;
			CurrentRotationRate = FMath::Clamp<float>(CurrentRotationRate, 0, MaxRotationRate);
		}
	}
}

void AGEBaseShip::MoveTo(float axis)
{
	APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	check(controller);
	float targetX, targetY;
	bool isCurrentlyPressed;
	controller->GetInputTouchState(ETouchIndex::Touch1, targetX, targetY, isCurrentlyPressed);
	if (isCurrentlyPressed)
	{
		MoveToPoint = FVector2D(targetX,targetY);
		IsMoving = true;
		return;
	}
	else
	{
		if (controller->IsInputKeyDown(EKeys::LeftMouseButton))
		{
			if (controller->GetMousePosition(targetX, targetY))
			{
				MoveToPoint = FVector2D(targetX, targetY);
				IsMoving = true;
				return;
			}
		}
	}

	IsMoving = false;
}

void AGEBaseShip::FireGunDownMapping()
{
	FireGunToggle = true;
}

void AGEBaseShip::FireGunReleaseMapping()
{
	FireGunToggle = false;
}

void AGEBaseShip::FireSelectedGun()
{
	// Fire Current Weapon
	switch (SelectedGun)
	{
	case ESelectedGun::SG_Main:
		if (MainGun->FireGun(ShipBody->GetForwardVector()))
			GunFired(MainGun, SelectedGun);
		break;
	case ESelectedGun::SG_Secondary:
		if (SecondaryGun->FireGun(ShipBody->GetForwardVector()))
			GunFired(SecondaryGun, SelectedGun);
		break;
	default:
		break;
	}
}

void AGEBaseShip::OnShipDeath()
{
	Destroy();
}

void AGEBaseShip::WasTargetBy(AGEBaseShip * aggresser)
{
	CurrentlyTargetingMe = aggresser;
}

void AGEBaseShip::MoveTo(AActor * Actor)
{
	FVector Location = Actor->GetActorLocation();
	MoveToPoint = FVector2D(Location.X, Location.Y);
	IsMoving = true;
}

bool AGEBaseShip::ShouldFireGun()
{
	if (CurrentlyTargetedShip != nullptr)
	{
		FVector CurrentLocation = GetActorLocation();
		FVector OtherActorLocation = CurrentlyTargetedShip->GetActorLocation();

		float distSq = FVector::DistSquared(CurrentLocation, OtherActorLocation);
		if (distSq <= (AttackDistance*AttackDistance))
		{
			FVector Direction = (OtherActorLocation - CurrentLocation).GetSafeNormal();
			float DotToTarget = FVector::DotProduct(Direction,ShipBody->GetForwardVector());
			
			if (DotToTarget >= FMath::Cos(FMath::DegreesToRadians(AttackAngle)))
			{
				return true;
			}
		}
	}

	return false;
}

bool AGEBaseShip::ShouldFireMainGun()
{
	return true; // base alwaus fire main gun
}

bool AGEBaseShip::ShouldFireSecondaryGun()
{
	return false; // Base always fires main gun
}

void AGEBaseShip::SearchForTarget(float radius)
{
	for (TActorIterator<AGEBaseShip> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AGEBaseShip *OtherShip = *ActorItr;
		if (this != OtherShip)
		{
			float sqrDistance = GetSquaredDistanceTo(OtherShip);
			if (OtherShip->CanBeTargetedBy(this) && sqrDistance <= (radius * radius))
			{
				CurrentlyTargetedShip = OtherShip;
				OtherShip->WasTargetBy(this);
			}
		}
	}

	if (!CurrentlyTargetedShip)
	{
		FVector Location = GetActorLocation();
		FVector WorldMoveTo = FVector(MoveToPoint.X, MoveToPoint.Y, Location.Z);
		if (FVector::DistSquared(Location,WorldMoveTo) <= 10.0)
		{
			float x = FMath::RandRange(-400, 400);
			float y = FMath::RandRange(-400, 400);
			MoveToPoint = FVector2D(Location.X + x, Location.Y + y);
			IsMoving = true;
		}
	}
}

bool AGEBaseShip::CanBeTargetedBy(AGEBaseShip* other)
{
	return true; // Currently no reason to not be allowed target
}

AGEBaseShip* AGEBaseShip::GetTarget()
{
	return CurrentlyTargetedShip;
}