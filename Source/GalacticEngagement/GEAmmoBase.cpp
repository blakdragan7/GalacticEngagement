// Fill out your copyright notice in the Description page of Project Settings.

#include "GEAmmoBase.h"
#include "Runtime/Engine/Classes/GameFramework/ProjectileMovementComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Components/ArrowComponent.h"
#include "DrawDebugHelpers.h"
#include "Interfaces/GEDamageInterface.h"

// Sets default values
AGEAmmoBase::AGEAmmoBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bWasLaunched = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("AmmoRoot"));
	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	AmmoMesh->SetupAttachment(RootComponent);
	AmmoMesh->SetWorldScale3D(FVector(0.1, 0.1, 0.1));

	/*FrontFacingArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("FrontFacingArrow"));
	FrontFacingArrow->SetupAttachment(AmmoMesh);
	FrontFacingArrow->bHiddenInGame = false;*/

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshCube(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (StaticMeshCube.Object)AmmoMesh->SetStaticMesh(StaticMeshCube.Object);
	
	// Setup Collision
	AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AmmoMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	AmmoMesh->OnComponentBeginOverlap.AddDynamic(this, &AGEAmmoBase::OnComponentOverlapBegin);
	//AmmoMesh->OnComponentEndOverlap.AddDynamic(this, &AGEAmmoBase::OnXXXOverlapEnd);

	TimeToLive = 10;
	InitialSpeed = 2000;
	MaxDamage = 10;

}

// Called when the game starts or when spawned
void AGEAmmoBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentDamage = MaxDamage;
}

// Called every frame
void AGEAmmoBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bWasLaunched)
	{
		FVector Location = GetActorLocation();
		AddActorWorldOffset(Velocity * DeltaTime);
		TimeToLive -= DeltaTime;
		if (TimeToLive <= 0)
		{
			Destroy();
		}
	}
}

void AGEAmmoBase::Launch(AActor* LaunchingActor, FVector Direction)
{
	Velocity = Direction * InitialSpeed;
	ignoredActor = LaunchingActor;
	bWasLaunched = true;
}

void AGEAmmoBase::OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (ignoredActor && IsValid(this) && bWasLaunched)
	{
		if (ignoredActor != OtherActor)
		{
			if (IGEDamageInterface* damageInterface = Cast<IGEDamageInterface>(OtherActor))
			{
				damageInterface->ReceiveDamage(CurrentDamage, OtherComp->GetComponentLocation());
				Destroy();
			}
		}
	}
}