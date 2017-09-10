// Fill out your copyright notice in the Description page of Project Settings.

#include "ComponentMountPoint.h"
#include "Runtime/Engine/Classes/Components/ArrowComponent.h"


// Sets default values for this component's properties
UComponentMountPoint::UComponentMountPoint()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UComponentMountPoint::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UComponentMountPoint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
}
