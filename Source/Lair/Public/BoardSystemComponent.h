// BoardSystemComponent.h
// Step 6: Board System Component (Grid Management)
// Manages the game board grid and tile spawning.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LairDataStructs.h"
#include "BoardSystemComponent.generated.h"

// Forward declarations
class ATile;
class AUnit;

/**
 * Component that manages the game board grid.
 * Responsibilities:
 * - Spawn grid of tiles from board layout data table
 * - Track tile positions in 2D grid
 * - Provide tile lookup by coordinate
 * - Calculate movement costs between tiles
 * - Identify neighboring tiles
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAIR_API UBoardSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBoardSystemComponent();

	virtual void BeginPlay() override;

	// ========================================================================
	// Configuration
	// ========================================================================

	/** Set the tile class to spawn */
	void SetTileClass(TSubclassOf<ATile> InTileClass) { TileClass = InTileClass; }

	/** Set the board layout data table */
	void SetBoardLayoutDataTable(UDataTable* InDataTable) { BoardLayoutDataTable = InDataTable; }

	/** Set the tile types data table */
	void SetTileTypesDataTable(UDataTable* InDataTable) { TileTypesDataTable = InDataTable; }

	// ========================================================================
	// Stable API - DO NOT MODIFY SIGNATURES
	// ========================================================================

	/**
	 * Initialize the board from a data table or generate default 10x10.
	 * @param LayoutTablePath - Path to board layout data table (empty for default)
	 */
	UFUNCTION(BlueprintCallable, Category = "Board")
	void InitializeBoard(const FString& LayoutTablePath);

	/**
	 * Get tile at specified grid coordinate.
	 * @param Coord - Grid coordinate (X, Y)
	 * @return Tile at coordinate or nullptr if not found
	 */
	UFUNCTION(BlueprintPure, Category = "Board")
	ATile* GetTileAt(FIntPoint Coord) const;

	/**
	 * Get all neighboring tiles of a tile.
	 * @param Tile - Source tile
	 * @return Array of neighboring tiles (up to 8)
	 */
	UFUNCTION(BlueprintPure, Category = "Board")
	TArray<ATile*> GetNeighborTiles(ATile* Tile) const;

	/**
	 * Get movement cost between two tiles.
	 * @param From - Starting coordinate
	 * @param To - Target coordinate
	 * @return Movement cost (1 for orthogonal, 2 for diagonal, -1 if invalid)
	 */
	UFUNCTION(BlueprintPure, Category = "Board")
	int32 GetMovementCost(FIntPoint From, FIntPoint To) const;

	// ========================================================================
	// Additional API
	// ========================================================================

	/**
	 * Get the base coordinate for a player.
	 * @param PlayerIndex - Player index (0 or 1)
	 * @return Grid coordinate of player's base
	 */
	UFUNCTION(BlueprintPure, Category = "Board")
	FIntPoint GetPlayerBaseCoord(int32 PlayerIndex) const;

	/**
	 * Check if a coordinate is valid (within grid bounds).
	 * @param Coord - Coordinate to check
	 * @return True if coordinate is valid
	 */
	UFUNCTION(BlueprintPure, Category = "Board")
	bool IsValidGridCoord(FIntPoint Coord) const;

	/**
	 * Get the current board size.
	 * @return Board dimensions
	 */
	UFUNCTION(BlueprintPure, Category = "Board")
	FIntPoint GetBoardSize() const { return BoardSize; }

protected:
	/** Grid storage (maps coordinate to tile) */
	UPROPERTY()
	TMap<FIntPoint, ATile*> TileGrid;

	/** Board dimensions */
	UPROPERTY()
	FIntPoint BoardSize;

	/** Tile class to spawn */
	UPROPERTY()
	TSubclassOf<ATile> TileClass;

	/** Board layout data table */
	UPROPERTY()
	UDataTable* BoardLayoutDataTable;

	/** Tile types data table */
	UPROPERTY()
	UDataTable* TileTypesDataTable;

	/** Player base coordinates */
	UPROPERTY()
	TArray<FIntPoint> PlayerBaseCoords;

	/** Spawn a tile at the given coordinate */
	ATile* SpawnTile(FIntPoint Coord, FName TileTypeID, int32 PlayerBaseIndex);

	/** Generate default 10x10 board */
	void GenerateDefaultBoard();

	/** Load board from data table */
	void LoadBoardFromDataTable();

	/** Check if movement is orthogonal */
	bool IsOrthogonalMove(FIntPoint From, FIntPoint To) const;

	/** Check if movement is diagonal */
	bool IsDiagonalMove(FIntPoint From, FIntPoint To) const;
};
