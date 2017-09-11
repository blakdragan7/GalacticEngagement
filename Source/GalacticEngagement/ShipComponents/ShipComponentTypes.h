#pragma once
#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EShipComponentType : uint8
{
	SC_Engine 			UMETA(DisplayName = "Engine"),
	SC_Thruster 		UMETA(DisplayName = "Thruster"),
	SC_MainGun 			UMETA(DisplayName = "Main Gun"),
	SC_SecondaryGun 	UMETA(DisplayName = "Secondary Gun")
};