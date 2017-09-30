// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShipComponentTypes.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Runtime/CoreUObject/Public/Templates/SubclassOf.h"
#include "ComponentMountPoint.generated.h"

UCLASS( ClassGroup=(ShipComponents), meta=(BlueprintSpawnableComponent) )
class GALACTICENGAGEMENT_API UComponentMountPoint : public UStaticMeshComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(Category = Debug, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* FrontFacingArrow;
	
	UPROPERTY(Category = Component, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Replicated)
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

	UPROPERTY(Category = Type, EditAnywhere, BlueprintReadWrite, Replicated)
	EShipComponentType AcceptedComponentType;

	UPROPERTY(Category = Tick, EditAnywhere, BlueprintReadWrite)
	bool ShouldTick;

	UFUNCTION(BlueprintCallable, Category = "Validity")
	FORCEINLINE bool HasBeenAssigned() {return AssignedComponent != 0;}

	UFUNCTION(BlueprintCallable, Category = "Component")
	FORCEINLINE class UShipComponentBase* GetComponentBase() { return AssignedComponent; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool AssignShipComponent(TSubclassOf<class UShipComponentBase> ComponentClass);
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool SizeTo(const FVector & inExtents);
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetEnabled(bool enabled);
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool SetComponent(class UShipComponentBase* inComponent);
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool RemoveComponent(class UShipComponentBase* outOldComponent);

	/* Used for saving and loading, Must be unique per ship, i.e. two ships with the 
	* same index is fine but more then one component on the same ship with same index is not fine.*/
	UPROPERTY(Category = Saving, EditAnywhere, BlueprintReadWrite)
	int32 index;

	/*Pre Casts*/
	/*AssignedComponent Pre casted to EngineComponent*/
	UPROPERTY(Category = PreCast, EditAnywhere, BlueprintReadWrite)
	class UGEEngineBaseComponent* EngineComponent;
	/*AssignedComponent Pre casted to GunComponent*/
	UPROPERTY(Category = PreCast, EditAnywhere, BlueprintReadWrite)
	class UGEGunBaseComponent*  GunComponent;
	/*AssignedComponent Pre casted to ThrusterComponent*/
	UPROPERTY(Category = PreCast, EditAnywhere, BlueprintReadWrite)
	class UGEThrusterBaseComponent*  ThrusterComponent;
};
