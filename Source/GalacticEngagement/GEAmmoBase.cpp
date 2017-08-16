// Fill out your copyright notice in the Description page of Project Settings.

#include "GEAmmoBase.h"
#include "Runtime/Engine/Classes/GameFramework/ProjectileMovementComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Components/ArrowComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AGEAmmoBase::AGEAmmoBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("AmmoRoot"));
	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	AmmoMesh->SetupAttachment(RootComponent);

	FrontFacingArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("FrontFacingArrow"));
	FrontFacingArrow->SetupAttachment(AmmoMesh);
	FrontFacingArrow->bHiddenInGame = false;

	AmmoMesh->SetWorldScale3D(FVector(0.1,0.1,0.1));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshCube(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (StaticMeshCube.Object)AmmoMesh->SetStaticMesh(StaticMeshCube.Object);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 0.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0;
	// setup movement
}

// Called when the game starts or when spawned
void AGEAmmoBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGEAmmoBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + (ProjectileMovement->Velocity.GetSafeNormal() * 100.0),
		100.f, FColor::Red, false, -1.f, (uint8)'\000', 10.f);
}

void AGEAmmoBase::Launch(FVector Velocity)
{
	ProjectileMovement->Velocity = Velocity;
}
