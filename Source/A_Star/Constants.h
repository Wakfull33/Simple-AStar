// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Constants.generated.h"

/**
 * 
 */
UCLASS()
class A_STAR_API UConstants : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
};

#pragma region  ENUMS

UENUM(BlueprintType)
enum FGroundTypes
{
	GP_EASY			UMETA(DisplayName = "Easy"),
	GP_DIFFICULT	UMETA(DisplayName = "Difficult"),
	GP_INSANE		UMETA(DisplayName = "Insane"),
	GP_IMPOSSIBLE	UMETA(DisplayName = "Impossible"),
	GP_NONE			UMETA(DisplayName = "None"),
};

#pragma endregion

#pragma region STRUCTS

USTRUCT(BlueprintType)
struct FTile
{
	GENERATED_BODY()
	
public:
	FTile()
	{
		GroundType = GP_NONE;
		WorldLocation = FVector(0, 0, 0);
		GridIndex = FVector2D(0, 0);
	};
	FTile(FGroundTypes groundType, FVector worldLocation, FVector2D gridIndex, int tileCost) : GroundType(groundType), WorldLocation(worldLocation), GridIndex(gridIndex), TileCost(tileCost){};

	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<FGroundTypes> GroundType;

	UPROPERTY(VisibleAnywhere)
	FVector WorldLocation;

	UPROPERTY(VisibleAnywhere)
	FVector2D GridIndex;

	UPROPERTY(VisibleAnywhere)
	int TileCost = 0;
	
	UPROPERTY(VisibleAnywhere)
	int FinalCost = 0;
	
	UPROPERTY(VisibleAnywhere)
	int CostFromStart = 0;

	UPROPERTY(VisibleAnywhere)
	int EstimatedCostToTarget = 0;

	UPROPERTY(VisibleAnywhere)
	FVector2D PreviousTile;

	UPROPERTY(VisibleAnywhere)
	class AGridTile* TileActor = nullptr;
};

#pragma endregion