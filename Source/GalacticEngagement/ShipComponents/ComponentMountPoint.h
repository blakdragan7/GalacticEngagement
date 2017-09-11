// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShipComponentTypes.h"
#include "Components/StaticMeshComponent.h"
#include "ComponentMountPoint.generated.h"

UCLASS( ClassGroup=(ShipComponents), meta=(BlueprintSpawnableComponent) )
class GALACTICENGAGEMENT_API UComponentMountPoint : public UStaticMeshComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(Category = Debug, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* FrontFacingArrow;
	UPROPERTY(Category = Debug, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UShipComponentBase* AssignedComponent;

public:	
	// Sets default values for this component's properties
	UComponentMountPoint();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(Category = Type, EditAnywhere, BlueprintReadWrite)
	EShipComponentType AcceptedComponentType;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool SetComponent(class UShipComponentBase* inComponent);
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool RemoveComponent(class UShipComponentBase* outOldComponent=0);
};
