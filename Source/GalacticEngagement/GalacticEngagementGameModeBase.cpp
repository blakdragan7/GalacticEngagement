// Fill out your copyright notice in the Description page of Project Settings.

#include "GalacticEngagementGameModeBase.h"
#include "GEBaseShip.h"
AGalacticEngagementGameModeBase::AGalacticEngagementGameModeBase()
{
	DefaultPawnClass = AGEBaseShip::StaticClass();
}