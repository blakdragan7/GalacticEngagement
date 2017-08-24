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
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
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

	ShipWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Ship HUD"));
	ShipWidget->SetupAttachment(RootComponent);

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

	GroundZ = 112;

	CameraOffsetScale = 2;

	AttackAngle = 45.0;
	AttackDistance = 1000.0;

	MaxHealth = 100;
	CurrentHealth = 0;

	CurrentlyTargetedShip = 0;

	HasMovementInput = false;

	SelectedGun = ESelectedGun::SG_Main;
}

// Called when the game starts or when spawned
void AGEBaseShip::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	Engine->SetControlledShip(this);
	if (ShipHUDWidget) { ShipWidget->SetWidgetClass(ShipHUDWidget); }
}

// Called every frame
void AGEBaseShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateInputs();
	UpdateCameraPosition();
	if (FireGunToggle)FireSelectedGun(); // Up to Individual Guns to Limit Fire rate

	DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + (ShipBody->GetForwardVector() * 100.0),
		100.f, FColor::Red, false, -1.f, (uint8)'\000', 10.f);
	
}

void AGEBaseShip::UpdateInputs()
{
	if (HasMovementInput)
	{
		FVector2D ScreenMoveToPoint;

		if (APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			float targetX, targetY;
			bool isCurrentlyPressed;
			controller->GetInputTouchState(ETouchIndex::Touch1, targetX, targetY, isCurrentlyPressed);
			if (isCurrentlyPressed)
			{
				ScreenMoveToPoint = FVector2D(targetX, targetY);
			}
			else
			{
				if (controller->IsInputKeyDown(EKeys::LeftMouseButton))
				{
					if (controller->GetMousePosition(targetX, targetY))
					{
						ScreenMoveToPoint = FVector2D(targetX, targetY);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Could Not Get Mouse Position !"));
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Could Not Get PlayerController !"));
			return;
		}

		if (APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			FVector WorldLocation;
			FVector WorldDirection;
			FVector WorldMoveToLocation;
			if (UGameplayStatics::DeprojectScreenToWorld(controller, ScreenMoveToPoint, WorldLocation, WorldDirection))
			{
				FVector EndLocation = WorldLocation - (WorldDirection * (WorldLocation.Z - GroundZ));
				WorldMoveToLocation = FMath::LinePlaneIntersection(WorldLocation, EndLocation, FVector(0, 0, GroundZ), FVector(0, 0, 1));
				Engine->MoveTo(WorldMoveToLocation);
				//if(GEngine)GEngine->AddOnScreenDebugMessage(1,15.0f,FColor::Yellow,FString::Printf(TEXT("World Location = %s"),*WorldMoveToLocation.ToString()));
			}
			else
			{
				// Failed to convert to world position
				UE_LOG(LogTemp, Warning, TEXT("Failed To convert screen to word position !"));
			}
		}
	}
}

// Called to bind functionality to input
void AGEBaseShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAction("MovementInput", IE_Pressed, this, &AGEBaseShip::MoveToDown);
	InputComponent->BindAction("MovementInput", IE_Released, this, &AGEBaseShip::MoveToUp);
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
	if (AllCurrentlyTargetingMe.Num() > 0)
	{
		AGEBaseShip* CurrentlyTargetingMe = AllCurrentlyTargetingMe[0];

		if (IsValid(CurrentlyTargetingMe))
		{
			float distance = GetDistanceTo(CurrentlyTargetingMe) * CameraOffsetScale;
			distance = FMath::Max<float>(distance, 600.0);
			CameraBoom->TargetArmLength = distance;
		}
		else
		{
			AllCurrentlyTargetingMe.Remove(CurrentlyTargetingMe);
		}
	}
	else
	{
		CameraBoom->TargetArmLength = 600.0;
	}
}
void AGEBaseShip::MoveToUp()
{
	HasMovementInput = false;
	Engine->StopMoving();
}

void AGEBaseShip::MoveToDown()
{
	HasMovementInput = true;
}

FVector AGEBaseShip::GetCurrentForwardVector()
{
	check(ShipBody);
	return ShipBody->GetForwardVector();
}

FRotator AGEBaseShip::GetCurrentRotation()
{
	check(ShipBody);
	return ShipBody->GetComponentRotation();
}

void AGEBaseShip::SetRotation(FRotator rotation)
{
	check(ShipBody);
	ShipBody->SetWorldRotation(rotation);
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
	AllCurrentlyTargetingMe.AddUnique(aggresser);
	AllCurrentlyTargetingMe.Sort([&](const AGEBaseShip& LHS, const AGEBaseShip& RHS) 
	{ 
		float dstl = GetSquaredDistanceTo(&LHS);
		float dstr = GetSquaredDistanceTo(&RHS);
		return dstl > dstr;
	});
}

float AGEBaseShip::GetHealthPercentage()
{
	return (float)CurrentHealth / (float)MaxHealth;
}

void AGEBaseShip::MoveTo(AActor * Actor)
{
	Engine->MoveTo(Actor->GetActorLocation());
}

void AGEBaseShip::InvalidateTarget()
{
	if (CurrentlyTargetedShip)
	{
		CurrentlyTargetedShip = false;
	}
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
		const FVector WorldMoveTo = Engine->GetMoveToLocation();
		if (FVector::DistSquared(Location,WorldMoveTo) <= 10.0)
		{
			float x = FMath::RandRange(-400, 400);
			float y = FMath::RandRange(-400, 400);
			Engine->MoveTo(FVector(x,y,Location.Z));
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