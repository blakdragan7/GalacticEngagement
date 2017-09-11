// Fill out your copyright notice in the Description page of Project Settings.

#include "ComponentMountPoint.h"
#include "ShipComponentBase.h"
#include "Runtime/Engine/Classes/Components/ArrowComponent.h"


// Sets default values for this component's properties
UComponentMountPoint::UComponentMountPoint()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	ShouldTick = false;
	AssignedComponent = 0;
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
	if (AssignedComponent)AssignedComponent->TickComponent(DeltaTime, TickType);
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
		AssignedComponent = inComponent;
		SetStaticMesh(AssignedComponent->GetModel());
		return true;
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
