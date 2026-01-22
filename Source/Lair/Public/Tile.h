// Tile.h
// Step 3: Tile Actor (Board Building Block)
// Represents one grid square on the board.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LairDataStructs.h"
#include "Tile.generated.h"

// Forward declaration
class AUnit;

/**
 * Represents a single tile on the game board.
 * Responsibilities:
 * - Represent one grid square
 * - Track 4 sub-slots (quarters)
 * - Track tile type (empty, base, etc.)
 * - Visual mesh representation
 */
UCLASS()
class LAIR_API ATile : public AActor
{
	GENERATED_BODY()

public:
	ATile();

	virtual void BeginPlay() override;

	// ========================================================================
	// Properties
	// ========================================================================

	/** Grid position (X, Y) on the board */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	FIntPoint GridCoord;

	/** Tile type ID (row name from DT_TileTypes) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	FName TileTypeID;

	/** Player base index (-1 if not a base, 0 for P1, 1 for P2) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	int32 PlayerBaseIndex = -1;

	// ========================================================================
	// Visual Components
	// ========================================================================

	/** Root component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootSceneComponent;

	/** Tile mesh (plane) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* TileMesh;

	// ========================================================================
	// Stable API - DO NOT MODIFY SIGNATURES
	// ========================================================================

	/**
	 * Check if a unit of the given size can be placed on this tile.
	 * @param SubSlotSize - Size of the unit (1 for normal, 2 for wagons)
	 * @return True if enough contiguous sub-slots are available
	 */
	UFUNCTION(BlueprintCallable, Category = "Tile")
	bool CanPlaceUnit(int32 SubSlotSize) const;

	/**
	 * Get the number of available (empty) sub-slots on this tile.
	 * @return Number of empty sub-slots (0-4)
	 */
	UFUNCTION(BlueprintPure, Category = "Tile")
	int32 GetAvailableSubSlots() const;

	/**
	 * Place a unit in a specific sub-slot.
	 * @param Unit - The unit to place
	 * @param SubSlotIndex - Which sub-slot (0-3)
	 * @return True if placement succeeded
	 */
	UFUNCTION(BlueprintCallable, Category = "Tile")
	bool PlaceUnitInSubSlot(AUnit* Unit, int32 SubSlotIndex);

	/**
	 * Remove a unit from this tile.
	 * @param Unit - The unit to remove
	 * @return True if removal succeeded
	 */
	UFUNCTION(BlueprintCallable, Category = "Tile")
	bool RemoveUnitFromSubSlot(AUnit* Unit);

	/**
	 * Get all units currently on this tile.
	 * @return Array of units on this tile
	 */
	UFUNCTION(BlueprintPure, Category = "Tile")
	TArray<AUnit*> GetAllUnitsOnTile() const;

	// ========================================================================
	// Additional API
	// ========================================================================

	/**
	 * Initialize the tile with grid coordinates and type.
	 * @param InGridCoord - Grid position
	 * @param InTileTypeID - Tile type from data table
	 */
	UFUNCTION(BlueprintCallable, Category = "Tile")
	void Initialize(FIntPoint InGridCoord, FName InTileTypeID);

	/**
	 * Get the tile type ID.
	 * @return Tile type name
	 */
	UFUNCTION(BlueprintPure, Category = "Tile")
	FName GetTileTypeID() const { return TileTypeID; }

	/**
	 * Set the tile's debug color.
	 * @param Color - New color for the tile
	 */
	UFUNCTION(BlueprintCallable, Category = "Tile")
	void SetTileColor(FLinearColor Color);

	/**
	 * Set highlight state for selection feedback.
	 * @param bHighlighted - Whether to show highlight
	 * @param Color - Highlight color
	 */
	UFUNCTION(BlueprintCallable, Category = "Tile")
	void SetHighlight(bool bHighlighted, FLinearColor Color = FLinearColor::Green);

	/**
	 * Set tile type data (called by BoardSystem during spawn).
	 * Used for DebugColor and other tile type properties.
	 * @param InTileTypeData - Tile type data from data table
	 */
	UFUNCTION(BlueprintCallable, Category = "Tile")
	void SetTileTypeData(const FTileTypeData& InTileTypeData);

	/**
	 * Find an available sub-slot for a unit of the given size.
	 * @param SubSlotSize - Size of the unit (1 for normal, 2 for wagons)
	 * @return Index of available slot, or -1 if none available
	 */
	UFUNCTION(BlueprintCallable, Category = "Tile")
	int32 FindAvailableSubSlot(int32 SubSlotSize) const;

protected:
	/** Units occupying each sub-slot (index 0-3) */
	UPROPERTY()
	TArray<AUnit*> SubSlots;

	/** Dynamic material instance for color changes */
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	/** Cached tile type data for visuals (DebugColor, etc.) */
	FTileTypeData CachedTileTypeData;

	/** Update visual representation */
	void UpdateVisuals();

	/** Get sub-slot world position offset */
	FVector GetSubSlotOffset(int32 SubSlotIndex) const;
};
