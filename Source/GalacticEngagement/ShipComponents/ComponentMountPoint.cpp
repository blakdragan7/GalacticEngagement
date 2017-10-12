// Fill out your copyright notice in the Description page of Project Settings.

#include "ComponentMountPoint.h"
#include "GalacticEngagement.h"
#include "ShipComponentBase.h"
#include "GEEngineBaseComponent.h"
#include "GEThrusterBaseComponent.h"
#include "GEGunBaseComponent.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Runtime/CoreUObject/Public/Templates/SubclassOf.h"
#include "Runtime/Engine/Classes/Components/ArrowComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

// Sets default values for this component's properties
UComponentMountPoint::UComponentMountPoint()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	ShouldTick = true;
	AssignedComponent = 0;
	index = -1;


	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshCube(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (StaticMeshCube.Object)SetStaticMesh(StaticMeshCube.Object);
}


// Called when the game starts
void UComponentMountPoint::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UComponentMountPoint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	if (!ShouldTick)return;
	if (AssignedComponent)AssignedComponent->TickComponent(DeltaTime);
}

void UComponentMountPoint::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	DOREPLIFETIME(UComponentMountPoint, AssignedComponent);
}

bool UComponentMountPoint::AssignShipComponent(TSubclassOf<class UShipComponentBase> ComponentClass)
{
	if (UShipComponentBase* component = NewObject<UShipComponentBase>(this, ComponentClass))
	{
		if (SetComponent(component) == false)
		{
			component->ConditionalBeginDestroy();
			return false;
		}
		return true;
	}

	return false;
}

bool UComponentMountPoint::SizeTo(const FVector & inExtents)
{
	if (AssignedComponent == 0 || GetStaticMesh() == 0)
	{
		UE_LOG(GELog,Warning,TEXT("UComponentMountPoint: Can not Size Mount Point without Assigned Component with a static mesh !"));
		return false;
	}

	FVector Origin, BoxExtents;
	float SphereRadius=0;
	UKismetSystemLibrary::GetComponentBounds(this, Origin, BoxExtents, SphereRadius);

	SetWorldScale3D(inExtents / BoxExtents);

	return true;
}

void UComponentMountPoint::SetEnabled(bool enabled)
{
	ShouldTick = enabled;
}

bool UComponentMountPoint::SetComponent(UShipComponentBase * inComponent)
{
	if (AssignedComponent || !inComponent)return false;

	if (inComponent->GetType() == AcceptedComponentType)
	{
		if (inComponent->AssignToMountPoint(this))
		{
			AssignedComponent = inComponent;
			SetStaticMesh(AssignedComponent->GetModel());

			GunComponent = Cast<UGEGunBaseComponent>(inComponent);
			EngineComponent = Cast<UGEEngineBaseComponent>(inComponent);
			ThrusterComponent = Cast<UGEThrusterBaseComponent>(inComponent);

			return true;
		}
	}
	return false;
}

bool UComponentMountPoint::RemoveComponent(class UShipComponentBase* outOldComponent)
{
	outOldComponent = 0;
	if (AssignedComponent == 0)return false;
	outOldComponent = AssignedComponent;
	AssignedComponent = 0;
	return true;
}
