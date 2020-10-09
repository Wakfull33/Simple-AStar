// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Obstacle.h"
#include "GridTile.h"


// Sets default values
AGrid::AGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	GridLocation = RootComponent->GetComponentLocation();

	PossibleNeighbors.Emplace(FVector2D(1, 0));
	PossibleNeighbors.Emplace(FVector2D(0, 1));
	PossibleNeighbors.Emplace(FVector2D(-1, 0));
	PossibleNeighbors.Emplace(FVector2D(0, -1));
}

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	Super::BeginPlay();

	DrawTile();
	SpawnTiles();
	AStarSearch(StartPosition, GoalPosition);
	DrawPath(RetracePath(GetTileFromWorldPosition(StartPosition).GridIndex, GetTileFromWorldPosition(GoalPosition).GridIndex));
}

// Called every frame
void AGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGrid::OnConstruction(const FTransform& Transform)
{
	FlushPersistentDebugLines(GetWorld());
	DrawDebugBox(GetWorld(), GetActorLocation(), FVector(GridSize.Y, GridSize.X, 5.f), GridDebugColor, true, -1, 0, 10);
	DrawDebugBox(GetWorld(), GetGridBottomLeft(), FVector(10.0f, 10.0f, 5.f), BottomLeftDebugColor, true, -1, 0, 10);
	DrawDebugBox(GetWorld(), GetGridTopRight(), FVector(10.0f, 10.0f, 5.f), FColor::Green, true, -1, 0, 10);

	DrawTile();
}

void AGrid::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	if(IsValid(this))
	{
		FlushPersistentDebugLines(GetWorld());
		DrawDebugBox(GetWorld(), GetActorLocation(), FVector(GridSize.Y, GridSize.X, 5.f), GridDebugColor, true, -1, 0, 10);
		DrawDebugBox(GetWorld(), GetGridBottomLeft(), FVector(10.0f, 10.0f, 5.f), BottomLeftDebugColor, true, -1, 0, 10);
		DrawDebugBox(GetWorld(), GetGridTopRight(), FVector(10.0f, 10.0f, 5.f), FColor::Green, true, -1, 0, 10);

		DrawDebugPoint(GetWorld(), GetTileFromWorldPosition(StartPosition).WorldLocation, 10, FColor::Blue, true);
		DrawDebugPoint(GetWorld(), GetTileFromWorldPosition(GoalPosition).WorldLocation, 10, FColor::Red, true);

		DrawTile();
	}
}

FVector AGrid::GetGridBottomLeft()
{
	FVector Location;

	Location = GetActorLocation() - GetActorRightVector() * GridSize.X;
	Location -= GetActorForwardVector() * GridSize.Y;

	return Location;
}

FVector AGrid::GetGridTopRight()
{
	FVector Location;

	Location = GetActorLocation() + GetActorRightVector() * GridSize.X;
	Location += GetActorForwardVector() * GridSize.Y;

	return Location;
}

FIntPoint AGrid::GetTileNumber()
{
	FIntPoint tileNumber;
	tileNumber = FIntPoint(FMath::RoundToInt(GridSize.X / TileSize), FMath::RoundToInt(GridSize.Y / TileSize));

	return tileNumber;
}

void AGrid::DrawTile()
{
	FIntPoint tileNumber = GetTileNumber();
	GridTiles.Reset();
	GridTiles.Reserve(tileNumber.X * tileNumber.Y);
	
	for(int y = 0; y < tileNumber.Y; y++)
	{
		for (int x = 0; x < tileNumber.X; x++)
		{
			FVector TilePosition = GetGridBottomLeft();
			TilePosition += GetActorRightVector() * (x * (TileSize * 2) + TileSize);
			TilePosition += GetActorForwardVector() * (y * (TileSize * 2) + TileSize);

			//UE_LOG(LogTemp, Warning, TEXT("TilePosition: %s    GridBottomLeft: %s"), *TilePosition.ToString(), *GetGridBottomLeft().ToString())

			FHitResult FirstDetection = TileDetection(TilePosition, ECC_GameTraceChannel1);
			if(FirstDetection.bBlockingHit)
			{
				FHitResult SecondDetection = TileDetection(TilePosition, ECC_GameTraceChannel2);
				if(!SecondDetection.bBlockingHit)
				{
					if(GetTileDebug)
						UKismetSystemLibrary::DrawDebugPlane(GetWorld(), FPlane(0.0f, 0.0f, 1.0f, 0.0f), TilePosition, TileSize * 0.9, GetColorByGroundTypes(FGroundTypes::GP_EASY), 600);
					GridTiles.Add(FTile(FGroundTypes::GP_EASY, TilePosition, FVector2D(x, y), GetTileCost(FGroundTypes::GP_EASY)));
				}
				else
				{
					AObstacle* obstacle = Cast<AObstacle>(SecondDetection.Actor.Get());

					if (GetTileDebug)
						UKismetSystemLibrary::DrawDebugPlane(GetWorld(), FPlane(0.0f, 0.0f, 1.0f, 0.0f), TilePosition, TileSize * 0.9, GetColorByGroundTypes(obstacle->GroundType), 600);
					GridTiles.Add(FTile(obstacle->GroundType, TilePosition, FVector2D(x, y), GetTileCost(obstacle->GroundType)));
				}
			}
			else
			{
				if (GetTileDebug)
					UKismetSystemLibrary::DrawDebugPlane(GetWorld(), FPlane(0.0f, 0.0f, 1.0f, 0.0f), TilePosition, TileSize * 0.9, GetColorByGroundTypes(FGroundTypes::GP_NONE), 600);
				GridTiles.Add(FTile(FGroundTypes::GP_NONE, TilePosition, FVector2D(x, y), GetTileCost(FGroundTypes::GP_NONE)));
			}
		}
	}
}

FHitResult AGrid::TileDetection(FVector TilePosition, ECollisionChannel Channel)
{
	FCollisionShape sphereTrace = FCollisionShape::MakeSphere(TileSize * 0.9);
	FHitResult hitResult;

	GetWorld()->SweepSingleByChannel(hitResult, TilePosition, TilePosition, FQuat::Identity, Channel, sphereTrace);

	if(GetCollisionDebug)
		DrawDebugSphere(GetWorld(), TilePosition, TileSize * 0.9f, 16, hitResult.bBlockingHit ? FColor::Green : FColor::Red, true);

	return hitResult;
}

FLinearColor AGrid::GetColorByGroundTypes(FGroundTypes groundType)
{
	FLinearColor color;
	
	switch(groundType)
	{
		case FGroundTypes::GP_EASY:
			color = FLinearColor(0.2f, 1.0f, 0.2f, 1.0f);
			break;
		case FGroundTypes::GP_DIFFICULT:
			color = FLinearColor(1.0f, 0.3f, 0.0f, 1.0f);
			break;
		case FGroundTypes::GP_INSANE:
			color = FLinearColor(1.0f, 0.8f, 0.0f, 1.0f);
			break;
		case FGroundTypes::GP_IMPOSSIBLE:
			color = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		default:
			color = FLinearColor(0.2f, 0.2f, 0.2f, 1.0f);
			break;
	}
	
	return  color;
}

void AGrid::SpawnTiles()
{
	for(FTile& elem : GridTiles)
	{
		if(GridTileBP)
		{
			const FVector* position = &elem.WorldLocation;
			const FRotator* rotation = &FRotator::ZeroRotator;
			if(elem.GroundType != FGroundTypes::GP_NONE)
			{
				AGridTile* tile = Cast<AGridTile>(GetWorld()->SpawnActor(GridTileBP.Get(), position, rotation, FActorSpawnParameters()));
				elem.TileActor = tile;
				elem.TileActor->SetTileActorColor(GetColorByGroundTypes(elem.GroundType));
				elem.TileActor->StaticMeshComponent->SetWorldScale3D(FVector(TileSize / 100, TileSize / 100, 1.0f));
			}
		}
	}
}

bool AGrid::IsPositionValid(FVector position)
{
	return (position.X > -GridSize.X / 2 && position.X <  GridSize.X / 2 && position.Y > -GridSize.Y / 2 && position.Y < GridSize.Y / 2);
}

bool AGrid::IsIndexValid(FVector2D index)
{
	return (index.X < GetTileNumber().X && index.X >= 0 && index.Y < GetTileNumber().Y && index.Y >= 0);
}

const FTile& AGrid::GetTileFromWorldPosition(FVector position)
{
	if(IsPositionValid(position))
	{
		int X = FMath::RoundToInt((position.X + GridSize.X / 2) / TileSize);
		int Y = FMath::RoundToInt((position.Y + GridSize.Y / 2) / TileSize);

		//UE_LOG(LogTemp, Warning, TEXT("Position: %s    LocationResize: %s    GridBottomLeft: %s"), *position.ToString(), *LocationResized.ToString(), *GetGridBottomLeft().ToString())
		UE_LOG(LogTemp, Warning, TEXT("Num: %d"), X + Y * GetTileNumber().Y)
		return GridTiles[X + Y * GetTileNumber().Y];
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OUT OF GRID BOUNDS !!!"))
		return GridTiles[0];
	}
}

int AGrid::GetTileCost(FGroundTypes groundType)
{
	int cost;

	switch (groundType)
	{
	case FGroundTypes::GP_EASY:
		cost = 1;
		break;
	case FGroundTypes::GP_DIFFICULT:
		cost = 2;
		break;
	case FGroundTypes::GP_INSANE:
		cost = 5;
		break;
	default:
		cost = 999;
		break;
	}

	return  cost;
}

void AGrid::InitTilesInfos(FVector2D Start, FVector2D End)
{
	for(auto& elem : GridTiles)
	{
		elem.FinalCost = 999;
		elem.CostFromStart = 999;
		elem.EstimatedCostToTarget = 999;
	}

	GridTiles[0].CostFromStart = 0;
	GridTiles[0].EstimatedCostToTarget = GetEstimatedCostToTarget(Start, End);
	GridTiles[0].FinalCost = GridTiles[0].EstimatedCostToTarget;
}

TArray<FVector2D> AGrid::RetracePath(FVector2D End, FVector2D Start)
{
	FVector2D CurrentIndex;
	FVector2D StartIndex;
	int Index = 0;
	TArray<FVector2D> Path;

	CurrentIndex = Start;
	StartIndex = End;
	
	while(CurrentIndex != StartIndex)
	{
		Path.Add(CurrentIndex);
		Index = CurrentIndex.X + CurrentIndex.Y * GetTileNumber().X;
		CurrentIndex = GridTiles[Index].PreviousTile;
		UE_LOG(LogTemp, Warning, TEXT("Retrace Add:%s "), *CurrentIndex.ToString());
		GridTiles[Index].TileActor->SetTileActorColor(FLinearColor::Black);
	}

	return Path;
}

void AGrid::DrawPath(TArray<FVector2D> Path)
{
	for(int i = 0; i < Path.Num() - 1; ++i)
	{
		FTile& tile1 = GridTiles[Path[i].X + Path[i].Y * GetTileNumber().X];
		FTile& tile2 = GridTiles[Path[i+1].X + Path[i+1].Y * GetTileNumber().X];
		
		DrawDebugLine(GetWorld(), tile1.WorldLocation, tile2.WorldLocation, FColor::Magenta, false, 60);
	}
}

TArray<FVector2D> AGrid::GetTileNeighbors(FVector2D gridIndex)
{
	TArray<FVector2D> neighbors;
	
	for(auto elem : PossibleNeighbors)
	{
		if (IsIndexValid(FVector2D(gridIndex.X + elem.X, gridIndex.Y + elem.Y)))
		{
			FTile& neighborTile = GridTiles[gridIndex.X + elem.X + (gridIndex.Y + elem.Y) * GetTileNumber().X];
			
			if(neighborTile.GroundType != FGroundTypes::GP_NONE && neighborTile.GroundType != FGroundTypes::GP_IMPOSSIBLE)
			{
				neighbors.Add(elem);
			}
		}
	}

	return neighbors;
}

int AGrid::GetEstimatedCostToTarget(FVector2D currentTile, FVector2D targetTile)
{
	FVector2D cost = currentTile - targetTile;

	return FMath::RoundToInt(FMath::Abs(cost.X) + FMath::Abs(cost.Y));
}

void AGrid::AStarSearch(FVector start, FVector goal)
{
	//TMap<FTile, int> frontier;
	FIntPoint TileNumber = GetTileNumber();
	
	if (IsPositionValid(start) && IsPositionValid(goal))
	{
		FVector2D Start = GetTileFromWorldPosition(start).GridIndex;
		FVector2D Goal = GetTileFromWorldPosition(goal).GridIndex;
		TArray<FVector2D> Opened;
		FVector2D OpenedCheapest;
		TArray<FVector2D> Closed;
		FVector2D CurrentIndex;

		InitTilesInfos(Start, Goal);
		
		Opened.Add(Start);

		while(Opened.Num() > 0)
		{
			OpenedCheapest = Opened[0];

			for (auto elem : Opened)
			{
				if(GridTiles[elem.X + elem.Y * TileNumber.X].FinalCost < GridTiles[OpenedCheapest.X + OpenedCheapest.Y * TileNumber.X].FinalCost)
				{
					OpenedCheapest = elem;
				}
				else if(GridTiles[elem.X + elem.Y * TileNumber.X].FinalCost == GridTiles[OpenedCheapest.X + OpenedCheapest.Y * TileNumber.X].FinalCost)
				{
					if(GridTiles[elem.X + elem.Y * TileNumber.X].EstimatedCostToTarget < GridTiles[OpenedCheapest.X + OpenedCheapest.Y * TileNumber.X].EstimatedCostToTarget)
					{
						OpenedCheapest = elem;
					}
				}
			}
			//UE_LOG(LogTemp, Warning, TEXT("PathFinding Add:%s "), *OpenedCheapest.ToString());
			//DrawDebugPoint(GetWorld(), GridTiles[OpenedCheapest.X + OpenedCheapest.Y * GetTileNumber().X].WorldLocation, 10, FColor::Purple, true);
			CurrentIndex = OpenedCheapest;
			//GridTiles[CurrentIndex.X + CurrentIndex.Y * GetTileNumber().X].TileActor->SetTileActorColor(FLinearColor::Blue);
			Opened.Remove(CurrentIndex);
			Closed.Add(CurrentIndex);

			TArray<FVector2D> Neighbors = GetTileNeighbors(CurrentIndex);
			for (auto elem : Neighbors)
			{
				int neighbor = (CurrentIndex.X + elem.X) + (CurrentIndex.Y + elem.Y) * TileNumber.X;
				if (!Closed.Contains(GridTiles[neighbor].GridIndex)) {
					int costFromStart = GridTiles[neighbor].TileCost + GridTiles[CurrentIndex.X + CurrentIndex.Y * TileNumber.X].CostFromStart;
					if(!Opened.Contains(GridTiles[neighbor].GridIndex))
					{
						Opened.Add(GridTiles[neighbor].GridIndex);
					}
					if(costFromStart > GridTiles[neighbor].CostFromStart)
					{
						GridTiles[neighbor].CostFromStart = costFromStart;
						GridTiles[neighbor].EstimatedCostToTarget = GetEstimatedCostToTarget(elem, Goal);
						GridTiles[neighbor].FinalCost = GridTiles[neighbor].EstimatedCostToTarget + GridTiles[neighbor].CostFromStart;
						GridTiles[neighbor].PreviousTile = CurrentIndex;
						if(GridTiles[neighbor].GridIndex == Goal)
						{
							UE_LOG(LogTemp, Warning, TEXT("Node Found: %d"), neighbor)
							return;
						}
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Closed already Contains: %s"), *elem.ToString())
				}
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Node Not Found: %d"), Closed.Num())
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Start or Goal is out of bound of the grid."));
	}
}