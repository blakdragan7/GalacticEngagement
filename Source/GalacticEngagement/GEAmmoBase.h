// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GEAmmoBase.generated.h"
/* Base Class for all projectile / attacks used by ships */
UCLASS()
class GALACTICENGAGEMENT_API AGEAmmoBase : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* AmmoMesh;
/*
	// debug components
	UPROPERTY(Category = Ship, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* FrontFacingArrow;
*/
	AActor* ignoredActor;

protected:
	UPROPERTY(Category = Collision, VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity;
	
public:	
	// Sets default values for this actor's properties
	AGEAmmoBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	UPROPERTY(Category = Collision, VisibleAnywhere, BlueprintReadOnly)
	bool bWasLaunched;
	UPROPERTY(Category = LifeTime, EditAnywhere, BlueprintReadWrite)
	float TimeToLive;
	UPROPERTY(Category = Movement, EditAnywhere, BlueprintReadWrite)
	float InitialSpeed;
	UPROPERTY(Category = Damage, EditAnywhere, BlueprintReadWrite)
	int32 MaxDamage;
	UPROPERTY(Category = Damage, EditAnywhere, BlueprintReadWrite)
	int32 CurrentDamage;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Launch(AActor* LaunchingActor, FVector Direction);
	
	UFUNCTION(Server, WithValidation, Reliable)
	virtual void Server_Launch(AActor* LaunchingActor, FVector Direction);

	UFUNCTION(NetMulticast, Reliable)
	virtual void MultiCast_Launch(AActor* LaunchingActor, FVector Direction);
	
	UFUNCTION()
	void OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(Server, WithValidation, Reliable)
	void Server_OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(NetMulticast, Reliable)
	void MultiCast_OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};

/*
UFUNCTION()
void OnXXXOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

UFUNCTION()
void OnXXXOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
*/