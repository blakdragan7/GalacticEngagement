// Fill out your copyright notice in the Description page of Project Settings.

#include "GETravelManagerBase.h"
#include "GESphericalTravelManager.h"

static GETravelManagerBase* Manager = 0;

GETravelManagerBase::GETravelManagerBase()
{
}

GETravelManagerBase::~GETravelManagerBase()
{
}

GETravelManagerBase * GETravelManagerBase::GetDefaultManager()
{
	if (Manager == 0)Manager = new GESphericalTravelManager(FVector(0,0,0),1000.0f);
	return Manager;
}
