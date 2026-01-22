// BoardSystemComponent.cpp
// Step 6: Board System Component (Grid Management)

#include "BoardSystemComponent.h"
#include "Tile.h"
#include "Unit.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"

UBoardSystemComponent::UBoardSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	BoardSize = FIntPoint(LairConstants::DEFAULT_BOARD_SIZE_X, LairConstants::DEFAULT_BOARD_SIZE_Y);

	// Initialize player base coordinates for 2-player game
	PlayerBaseCoords.Add(FIntPoint(0, 0));      // Player 1 at (0, 0)
	PlayerBaseCoords.Add(FIntPoint(9, 9));      // Player 2 at (9, 9)
}

void UBoardSystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBoardSystemComponent::InitializeBoard(const FString& LayoutTablePath)
{
	UE_LOG(LogTemp, Log, TEXT("UBoardSystemComponent::InitializeBoard - Starting board initialization"));

	// Clear existing tiles
	for (auto& Pair : TileGrid)
	{
		if (Pair.Value)
		{
			Pair.Value->Destroy();
		}
	}
	TileGrid.Empty();

	// Try to load from data table first
	if (BoardLayoutDataTable)
	{
		LoadBoardFromDataTable();
	}
	else
	{
		// Generate default 10x10 board
		GenerateDefaultBoard();
	}

	UE_LOG(LogTemp, Log, TEXT("UBoardSystemComponent::InitializeBoard - Board created with %d tiles"),
		TileGrid.Num());
}

void UBoardSystemComponent::GenerateDefaultBoard()
{
	BoardSize = FIntPoint(LairConstants::DEFAULT_BOARD_SIZE_X, LairConstants::DEFAULT_BOARD_SIZE_Y);

	for (int32 X = 0; X < BoardSize.X; ++X)
	{
		for (int32 Y = 0; Y < BoardSize.Y; ++Y)
		{
			FIntPoint Coord(X, Y);
			FName TileType = FName("Empty");
			int32 BaseIndex = -1;

			// Check if this is a player base
			if (X == 0 && Y == 0)
			{
				TileType = FName("PlayerBase");
				BaseIndex = 0;
			}
			else if (X == BoardSize.X - 1 && Y == BoardSize.Y - 1)
			{
				TileType = FName("PlayerBase");
				BaseIndex = 1;
			}

			SpawnTile(Coord, TileType, BaseIndex);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("UBoardSystemComponent::GenerateDefaultBoard - Generated %dx%d board"),
		BoardSize.X, BoardSize.Y);
}

void UBoardSystemComponent::LoadBoardFromDataTable()
{
	if (!BoardLayoutDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBoardSystemComponent::LoadBoardFromDataTable - No data table set"));
		GenerateDefaultBoard();
		return;
	}

	TArray<FBoardLayoutRow*> AllRows;
	BoardLayoutDataTable->GetAllRows<FBoardLayoutRow>(TEXT("LoadBoardFromDataTable"), AllRows);

	if (AllRows.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBoardSystemComponent::LoadBoardFromDataTable - Data table is empty"));
		GenerateDefaultBoard();
		return;
	}

	// Find board bounds
	int32 MaxX = 0, MaxY = 0;
	for (const FBoardLayoutRow* Row : AllRows)
	{
		MaxX = FMath::Max(MaxX, Row->GridCoord.X);
		MaxY = FMath::Max(MaxY, Row->GridCoord.Y);
	}
	BoardSize = FIntPoint(MaxX + 1, MaxY + 1);

	// Spawn tiles from data table
	for (const FBoardLayoutRow* Row : AllRows)
	{
		SpawnTile(Row->GridCoord, Row->TileTypeID, Row->PlayerBaseIndex);

		// Update player base coordinates
		if (Row->PlayerBaseIndex >= 0 && Row->PlayerBaseIndex < PlayerBaseCoords.Num())
		{
			PlayerBaseCoords[Row->PlayerBaseIndex] = Row->GridCoord;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("UBoardSystemComponent::LoadBoardFromDataTable - Loaded %d tiles from data table"),
		AllRows.Num());
}

ATile* UBoardSystemComponent::SpawnTile(FIntPoint Coord, FName TileTypeID, int32 PlayerBaseIndex)
{
	if (!TileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBoardSystemComponent::SpawnTile - TileClass not set, using default ATile"));
		TileClass = ATile::StaticClass();
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("UBoardSystemComponent::SpawnTile - World is null"));
		return nullptr;
	}

	// Calculate world position
	FVector WorldPosition;
	WorldPosition.X = Coord.X * LairConstants::TILE_WORLD_SIZE;
	WorldPosition.Y = Coord.Y * LairConstants::TILE_WORLD_SIZE;
	WorldPosition.Z = 0.0f;

	// Spawn tile
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ATile* NewTile = World->SpawnActor<ATile>(TileClass, WorldPosition, FRotator::ZeroRotator, SpawnParams);
	if (NewTile)
	{
		NewTile->Initialize(Coord, TileTypeID);
		NewTile->PlayerBaseIndex = PlayerBaseIndex;
		TileGrid.Add(Coord, NewTile);

		UE_LOG(LogTemp, Verbose, TEXT("UBoardSystemComponent::SpawnTile - Spawned tile at (%d, %d) type: %s base: %d"),
			Coord.X, Coord.Y, *TileTypeID.ToString(), PlayerBaseIndex);
	}

	return NewTile;
}

ATile* UBoardSystemComponent::GetTileAt(FIntPoint Coord) const
{
	ATile* const* TilePtr = TileGrid.Find(Coord);
	if (TilePtr)
	{
		return *TilePtr;
	}
	return nullptr;
}

TArray<ATile*> UBoardSystemComponent::GetNeighborTiles(ATile* Tile) const
{
	TArray<ATile*> Neighbors;

	if (!Tile)
	{
		return Neighbors;
	}

	FIntPoint Coord = Tile->GridCoord;

	// All 8 directions (orthogonal + diagonal)
	TArray<FIntPoint> Offsets = {
		FIntPoint(0, 1),   // N
		FIntPoint(0, -1),  // S
		FIntPoint(1, 0),   // E
		FIntPoint(-1, 0),  // W
		FIntPoint(1, 1),   // NE
		FIntPoint(-1, 1),  // NW
		FIntPoint(1, -1),  // SE
		FIntPoint(-1, -1)  // SW
	};

	for (const FIntPoint& Offset : Offsets)
	{
		FIntPoint NeighborCoord = Coord + Offset;
		ATile* NeighborTile = GetTileAt(NeighborCoord);
		if (NeighborTile)
		{
			Neighbors.Add(NeighborTile);
		}
	}

	return Neighbors;
}

int32 UBoardSystemComponent::GetMovementCost(FIntPoint From, FIntPoint To) const
{
	// Check if both coordinates are valid
	if (!IsValidGridCoord(From) || !IsValidGridCoord(To))
	{
		return -1;
	}

	// Same tile = no cost
	if (From == To)
	{
		return 0;
	}

	// Check if tiles are adjacent
	int32 DeltaX = FMath::Abs(To.X - From.X);
	int32 DeltaY = FMath::Abs(To.Y - From.Y);

	// Orthogonal movement (N/S/E/W) - cost 1
	if ((DeltaX == 1 && DeltaY == 0) || (DeltaX == 0 && DeltaY == 1))
	{
		return LairConstants::ORTHOGONAL_MOVE_COST;
	}

	// Diagonal movement (NE/NW/SE/SW) - cost 2
	if (DeltaX == 1 && DeltaY == 1)
	{
		return LairConstants::DIAGONAL_MOVE_COST;
	}

	// Not adjacent - invalid move
	return -1;
}

FIntPoint UBoardSystemComponent::GetPlayerBaseCoord(int32 PlayerIndex) const
{
	if (PlayerIndex >= 0 && PlayerIndex < PlayerBaseCoords.Num())
	{
		return PlayerBaseCoords[PlayerIndex];
	}

	UE_LOG(LogTemp, Warning, TEXT("UBoardSystemComponent::GetPlayerBaseCoord - Invalid player index %d"),
		PlayerIndex);
	return FIntPoint::ZeroValue;
}

bool UBoardSystemComponent::IsValidGridCoord(FIntPoint Coord) const
{
	return Coord.X >= 0 && Coord.X < BoardSize.X &&
	       Coord.Y >= 0 && Coord.Y < BoardSize.Y;
}

bool UBoardSystemComponent::IsOrthogonalMove(FIntPoint From, FIntPoint To) const
{
	int32 DeltaX = FMath::Abs(To.X - From.X);
	int32 DeltaY = FMath::Abs(To.Y - From.Y);
	return (DeltaX == 1 && DeltaY == 0) || (DeltaX == 0 && DeltaY == 1);
}

bool UBoardSystemComponent::IsDiagonalMove(FIntPoint From, FIntPoint To) const
{
	int32 DeltaX = FMath::Abs(To.X - From.X);
	int32 DeltaY = FMath::Abs(To.Y - From.Y);
	return DeltaX == 1 && DeltaY == 1;
}
