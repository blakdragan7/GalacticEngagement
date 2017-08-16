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
// Sets default values
AGEBaseShip::AGEBaseShip()
{
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

	SelectedGun = ESelectedGun::SG_Main;
}

// Called when the game starts or when spawned
void AGEBaseShip::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGEBaseShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FVector WorldLocation;
		FVector WorldDirection;
		if (UGameplayStatics::DeprojectScreenToWorld(controller, MoveToPoint, WorldLocation, WorldDirection))
		{
			FVector EndLocation = WorldLocation - (WorldDirection * (WorldLocation.Z - GroundZ));
			FVector WorldMoveToLocation = FMath::LinePlaneIntersection(WorldLocation, EndLocation, FVector(0, 0, GroundZ), FVector(0, 0, 1));
			//if(GEngine)GEngine->AddOnScreenDebugMessage(1,15.0f,FColor::Yellow,FString::Printf(TEXT("World Location = %s"),*WorldMoveToLocation.ToString()));

			FVector CurrentLocation = GetActorLocation();
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
			//if (GEngine)GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Yellow, FString::Printf(TEXT("CurrentRotationRate = %f"), CurrentRotationRate));

			SetActorLocation(CurrentLocation + (ForwardVector * CurrentSpeed * DeltaTime));
		}
		else
		{
			// Failed to convert to world position
		}
	}

	UpdateMovementRates(DeltaTime);
	if(FireGunToggle)FireSelectedGun(); // Up to Individual Guns to Limit Fire rate

	DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + (ShipBody->GetForwardVector() * 100.0),
		100.f, FColor::Red, false, -1.f, (uint8)'\000', 10.f);
}

// Called to bind functionality to input
void AGEBaseShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAxis("MoveTo", this, &AGEBaseShip::MoveTo);
	InputComponent->BindAction("MainFire", IE_Released, this, &AGEBaseShip::FireGunReleaseMapping);
	InputComponent->BindAction("MainFire", IE_Pressed, this, &AGEBaseShip::FireGunDownMapping);
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
		if (CurrentRotationRate < MaxRotationAccel)
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
