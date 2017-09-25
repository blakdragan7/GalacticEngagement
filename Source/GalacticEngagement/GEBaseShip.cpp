// Fill out your copyright notice in the Description page of Project Settings.

#include "GEBaseShip.h"
//#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "ShipComponents/GEGunBaseComponent.h"
#include "ShipComponents/GEEngineBaseComponent.h"
#include "ShipComponents/GEThrusterBaseComponent.h"
#include "ShipComponents/ComponentMountPoint.h"
#include "Math/GEGameStatistics.h"
#include "AI/GEBaseEnemyAIController.h"
#include "DrawDebugHelpers.h"
#include "Runtime/CoreUObject/Public/Templates/SubclassOf.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
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
	AIControllerClass = AGEBaseEnemyAIController::StaticClass();

	SetReplicates(true);

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
	CameraBoom->SetWorldRotation(FRotator(-90, 0, 0));
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller
	CameraBoom->bDoCollisionTest = false;
	//CameraBoom->CameraLagSpeed = 0;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	FrontFacingArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("FrontFacingArrow"));
	FrontFacingArrow->SetupAttachment(ShipBody);
	FrontFacingArrow->bHiddenInGame = false;

	ThrusterMount = CreateDefaultSubobject<UComponentMountPoint>(TEXT("Thruster"));
	ThrusterMount->SetupAttachment(ShipBody);
	ThrusterMount->AcceptedComponentType = EShipComponentType::SC_Thruster;
	ThrusterMount->index = 1;

	EngineMount = CreateDefaultSubobject<UComponentMountPoint>(TEXT("Engine"));
	EngineMount->SetupAttachment(ShipBody);
	EngineMount->AcceptedComponentType = EShipComponentType::SC_Engine;
	EngineMount->index = 0;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshCube(TEXT("StaticMesh'/Game/Models/BaseShipStart2.BaseShipStart2'"));
	if (StaticMeshCube.Object)ShipBody->SetStaticMesh(StaticMeshCube.Object);

	CameraOffsetScale = 2;

	StartingCameraArmLength = 1500;
	MaxCameraArmLength = 4500;

	AttackAngle = 45.0;
	AttackDistance = 1000.0;

	MaxHealth = 100;
	CurrentHealth = 0;

	CurrentlyTargetedShip = 0;
	CurrentCameraDistance = 0;

	HasMovementInput = false;
	HasCameraDistance = false;

	SelectedGun = ESelectedGun::SG_Main;
}

// Called when the game starts or when spawned
void AGEBaseShip::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	if (ShipHUDWidget) { ShipWidget->SetWidgetClass(ShipHUDWidget); }
}

void AGEBaseShip::OnConstruction(const FTransform & Transform)
{
	CameraBoom->TargetArmLength = StartingCameraArmLength; // The camera follows at this distance behind the character	
}

void AGEBaseShip::SetToMaxCaemraDistance()
{
	CurrentCameraDistance = CameraBoom->TargetArmLength;
	CameraBoom->TargetArmLength = MaxCameraArmLength;
}

void AGEBaseShip::ResetToNormalCameraDistance()
{
	if (HasCameraDistance)
	{
		CameraBoom->TargetArmLength = CurrentCameraDistance;
	}
	else
	{
		CameraBoom->TargetArmLength = StartingCameraArmLength; // The camera follows at this distance behind the character	
	}
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
		if(ThrusterMount->HasBeenAssigned())
		ThrusterMount->ThrusterComponent->MoveTo(ScreenMoveToPoint);
	}
}

// Called to bind functionality to input
void AGEBaseShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UE_LOG(LogTemp,Warning,TEXT("setup input"));
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
			distance = FMath::Max<float>(distance, StartingCameraArmLength);
			CameraBoom->TargetArmLength = FMath::Min<float>(distance,MaxCameraArmLength);
		}
		else
		{
			AllCurrentlyTargetingMe.Remove(CurrentlyTargetingMe);
		}
	}
	else
	{
		CameraBoom->TargetArmLength = StartingCameraArmLength;
	}
}
void AGEBaseShip::MoveToUp()
{
	if (!ThrusterMount->HasBeenAssigned())return;
	HasMovementInput = false;
	ThrusterMount->ThrusterComponent->StopMoving();
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
	return ShipBody->RelativeRotation;
}

FVector AGEBaseShip::GetCurrentLocation()
{
	return ShipBody->GetComponentLocation();
}

void AGEBaseShip::SetRotation(FQuat rotation)
{
	check(ShipBody);
	ShipBody->SetRelativeRotation(rotation);
}

void AGEBaseShip::SetRotation(FRotator rotation)
{
	check(ShipBody);
	//ShipBody->AddRelativeRotation(rotation);
	ShipBody->SetRelativeRotation(rotation);
}

void AGEBaseShip::FireGunDownMapping()
{
	FireGunToggle = true;
}

void AGEBaseShip::FireGunReleaseMapping()
{
	FireGunToggle = false;
}

bool AGEBaseShip::SetLocation(const FVector & NewLocation, bool bSweep, FHitResult &OutSweepHitResult)
{
	if (bSweep)
	{
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this);

		FVector Origin, Extents;
		GetActorBounds(true,Origin,Extents);
		FVector currentLocation = GetActorLocation();
		if (GetWorld()->SweepSingleByChannel(OutSweepHitResult, currentLocation, NewLocation, FQuat(), ECollisionChannel::ECC_WorldDynamic, FCollisionShape::MakeSphere(GEGameStatistics::MaxVectorComponent(Extents)), TraceParams))
		{
			SetActorLocation(OutSweepHitResult.Location + OutSweepHitResult.ImpactNormal * 10);
			return false;
		}
		else
		{
			SetActorLocation(NewLocation);
			return true;
		}
	
	}
	return SetActorLocation(NewLocation);
}

void AGEBaseShip::FireSelectedGun()
{
	// Fire Current Weapon
	switch (SelectedGun)
	{
	case ESelectedGun::SG_Main:
		for (auto gun : MainGunComponents)
		{
			if (gun->HasBeenAssigned())
			{
				gun->GunComponent->FireGun();
			}
		}
		break;
	case ESelectedGun::SG_Secondary:
		for (auto gun : SecondaryGunComponents)
		{
			if (gun->HasBeenAssigned())
			{
				gun->GunComponent->FireGun();
			}
		}
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

UComponentMountPoint * AGEBaseShip::ClosestMountToPoint(FVector2D screen_point, float range)
{
	UComponentMountPoint* closestPoint = 0;

	if (APlayerController* controller = Cast<APlayerController>(GetController()))
	{
		float rangeSqr = (range*range);

		float distance = 1000000;

		FVector2D ScreenLocation;
		FVector WorldLocation = EngineMount->GetComponentLocation();
		if (controller->ProjectWorldLocationToScreen(WorldLocation, ScreenLocation))
		{
			distance = (ScreenLocation - screen_point).SizeSquared();
			if (distance <= rangeSqr)
			{
				closestPoint = EngineMount;
			}
		}

		WorldLocation = ThrusterMount->GetComponentLocation();
		if (controller->ProjectWorldLocationToScreen(WorldLocation, ScreenLocation))
		{
			float ldistance = (ScreenLocation - screen_point).SizeSquared();
			if (ldistance <= rangeSqr && ldistance < distance)
			{
				closestPoint = ThrusterMount;
			}
		}

		for (auto mount : MainGunComponents)
		{
			WorldLocation = mount->GetComponentLocation();
			if (controller->ProjectWorldLocationToScreen(WorldLocation, ScreenLocation))
			{
				float ldistance = (ScreenLocation - screen_point).SizeSquared();
				if (ldistance <= rangeSqr && ldistance < distance)
				{
					closestPoint = mount;
				}
			}
		}

		for (auto mount : SecondaryGunComponents)
		{
			WorldLocation = mount->GetComponentLocation();
			if (controller->ProjectWorldLocationToScreen(WorldLocation, ScreenLocation))
			{
				float ldistance = (ScreenLocation - screen_point).SizeSquared();
				if (ldistance <= rangeSqr && ldistance < distance)
				{
					closestPoint = mount;
				}
			}
		}
	}
	return closestPoint;
}

void AGEBaseShip::AddVelocityEffector(AActor * effector)
{
	if (!ThrusterMount->HasBeenAssigned())return;
	ThrusterMount->ThrusterComponent->AddEffector(effector);
}

void AGEBaseShip::removeVelocityEffector(AActor * effector)
{
	if (!ThrusterMount->HasBeenAssigned())return;
	ThrusterMount->ThrusterComponent->RemoveEffector(effector);
}

void AGEBaseShip::AssignComponent(TSubclassOf<UShipComponentBase> component, EShipComponentType type, FVector2D inScreenPosition)
{
	switch (type)
	{
	case EShipComponentType::SC_Engine:
		EngineMount->AssignShipComponent(component);
		break;
	case EShipComponentType::SC_Thruster:
		ThrusterMount->AssignShipComponent(component);
		break;
	case EShipComponentType::SC_MainGun:
		break;
	case EShipComponentType::SC_SecondaryGun:
		break;
	}
}

float AGEBaseShip::GetHealthPercentage()
{
	return (float)CurrentHealth / (float)MaxHealth;
}

void AGEBaseShip::MoveTo(AActor * Actor)
{
	if (!ThrusterMount->HasBeenAssigned())return;
	ThrusterMount->ThrusterComponent->MoveTo(Actor->GetActorLocation());
}

void AGEBaseShip::InvalidateTarget()
{
	if (CurrentlyTargetedShip)
	{
		CurrentlyTargetedShip = 0;
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

	if (!CurrentlyTargetedShip && ThrusterMount->HasBeenAssigned())
	{
		FVector Location = GetActorLocation();
		const FVector WorldMoveTo = ThrusterMount->ThrusterComponent->GetMoveToLocation();
		if (FVector::DistSquared(Location,WorldMoveTo) <= 10.0)
		{
			float x = FMath::RandRange(-400, 400);
			float y = FMath::RandRange(-400, 400);
			ThrusterMount->ThrusterComponent->MoveTo(FVector(x,y,Location.Z));
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
