// Fill out your copyright notice in the Description page of Project Settings.

#include "GEDamageIndicator.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Components/TextRenderComponent.h"

AGEDamageIndicator::AGEDamageIndicator()
{
	PrimaryActorTick.bCanEverTick = true;
	GetTextRender()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetTextRender()->VerticalAlignment = EVerticalTextAligment::EVRTA_TextCenter;
	GetTextRender()->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
	TimeToLive = 1;
	WasSetup = false;
	speed = 400.0;
}

void AGEDamageIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (WasSetup)
	{
		TimeToLive -= DeltaTime;
		if (TimeToLive <= 0)
		{
			Destroy();
		}
		FVector Location = GetActorLocation();
		Location.X += speed * DeltaTime;
		SetActorLocation(Location);
	}
}

AGEDamageIndicator * AGEDamageIndicator::SpawnIndicatorWithDamageAndDuration(UObject* WorldContextObject, FVector Location,float Duration, FString Damage)
{
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FTransform transform;
	transform.SetLocation(Location);
	transform.SetRotation(FRotator(90,180,0).Quaternion());
	AGEDamageIndicator* DI = WorldContextObject->GetWorld()->SpawnActor<AGEDamageIndicator>(AGEDamageIndicator::StaticClass(),transform, params);
	DI->GetTextRender()->SetText(Damage);
	DI->GetTextRender()->SetTextRenderColor(FColor::Red);
	DI->SetActorScale3D(FVector(2.f, 2.f, 2.f));
	DI->TimeToLive = Duration;
	DI->WasSetup = true;
	return DI;
}
