// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/Map.h"
#include "Constants.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.generated.h"


class AGridTile;

UCLASS()
class A_STAR_API AGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid | Parameters")
	FVector GridLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid | Parameters")
	FVector2D GridSize = FVector2D(500.f, 500.f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid | Parameters")
	float TileSize = 25.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid | Parameters")
	TArray<FTile> GridTiles;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid | Parameters")
	TSubclassOf<class AGridTile> GridTileBP = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid | Parameters | Debug")
	FColor GridDebugColor = FColor(255, 0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid | Parameters | Debug")
	FColor BottomLeftDebugColor = FColor(0, 0, 255);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid | Parameters | Debug")
	bool GetCollisionDebug = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid | Parameters | Debug")
	bool GetTileDebug = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid | A Star Parameters")
	FVector StartPosition = FVector(150, -220, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid | A Star Parameters")
	FVector GoalPosition = FVector(190, 220, 0);

	TArray<FVector2D> PossibleNeighbors;



	
	
	UFUNCTION(BlueprintPure)
	FVector GetGridBottomLeft();

	UFUNCTION(BlueprintPure)
	FVector GetGridTopRight();
	
	UFUNCTION(BlueprintPure)
	FIntPoint GetTileNumber();

	UFUNCTION()
	void DrawTile();

	UFUNCTION()
	void SpawnTiles();
	
	UFUNCTION()
	FHitResult TileDetection(FVector TilePosition, ECollisionChannel Channel);

	UFUNCTION()
	FLinearColor GetColorByGroundTypes(FGroundTypes groundType);

	UFUNCTION()
	bool IsPositionValid(FVector position);

	UFUNCTION()
	bool IsIndexValid(FVector2D index);
	
	UFUNCTION()
	const FTile& GetTileFromWorldPosition(FVector position);

	UFUNCTION()
	int GetTileCost(FGroundTypes groundType);

	UFUNCTION()
	void InitTilesInfos(FVector2D Start, FVector2D End);

	UFUNCTION()
	TArray<FVector2D> RetracePath(FVector2D End, FVector2D Start);

	UFUNCTION()
	void DrawPath(TArray<FVector2D> Path);
	
	UFUNCTION()
	TArray<FVector2D> GetTileNeighbors(FVector2D gridIndex);

	UFUNCTION()
	int GetEstimatedCostToTarget(FVector2D currentTile, FVector2D targetTile);
	
	UFUNCTION()
	void AStarSearch(FVector start, FVector goal);
};