// RulesEngineComponent.h
// Step 8: Rules Engine Component (Validation Logic)
// Validates game actions according to rules.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LairDataStructs.h"
#include "RulesEngineComponent.generated.h"

// Forward declarations
class ATile;
class AUnit;

/**
 * Component that validates game actions according to rules.
 * Responsibilities:
 * - Validate unit purchases (gold check)
 * - Validate unit placement (sub-slot availability)
 * - Validate movement (Phase 2+)
 * - Validate actions based on current phase
 * - Load and cache unit data from Data Tables
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAIR_API URulesEngineComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URulesEngineComponent();

	// ========================================================================
	// Initialization
	// ========================================================================

	/**
	 * Initialize the rules engine with data tables.
	 * @param InUnitsTable - Unit stats data table
	 * @param InTileTypesTable - Tile types data table
	 */
	UFUNCTION(BlueprintCallable, Category = "Rules")
	void Initialize(UDataTable* InUnitsTable, UDataTable* InTileTypesTable);

	// ========================================================================
	// Stable API - DO NOT MODIFY SIGNATURES
	// ========================================================================

	/**
	 * Check if player can afford a unit.
	 * @param PlayerGold - Current gold amount
	 * @param UnitTypeID - Row name from DT_Units
	 * @return True if gold >= unit cost
	 */
	UFUNCTION(BlueprintPure, Category = "Rules")
	bool CanAffordUnit(int32 PlayerGold, FName UnitTypeID) const;

	/**
	 * Check if a unit can be spawned at a tile.
	 * @param Tile - Target tile
	 * @param SubSlotSize - Size of unit to spawn
	 * @return True if tile has enough space
	 */
	UFUNCTION(BlueprintPure, Category = "Rules")
	bool CanSpawnUnitAt(ATile* Tile, int32 SubSlotSize) const;

	/**
	 * Check if a specific unit can be placed at a tile.
	 * @param Tile - Target tile
	 * @param Unit - Unit to place
	 * @return True if placement is valid
	 */
	UFUNCTION(BlueprintPure, Category = "Rules")
	bool CanPlaceUnitAt(ATile* Tile, AUnit* Unit) const;

	// ========================================================================
	// Additional API
	// ========================================================================

	/**
	 * Get unit data from data table.
	 * @param UnitTypeID - Row name from DT_Units
	 * @return Unit data struct (empty if not found)
	 */
	UFUNCTION(BlueprintPure, Category = "Rules")
	FUnitData GetUnitData(FName UnitTypeID) const;

	/**
	 * Get tile type data from data table.
	 * @param TileTypeID - Row name from DT_TileTypes
	 * @return Tile type data struct (empty if not found)
	 */
	UFUNCTION(BlueprintPure, Category = "Rules")
	FTileTypeData GetTileTypeData(FName TileTypeID) const;

	/**
	 * Check if a tile is walkable.
	 * @param Tile - Tile to check
	 * @return True if units can move onto this tile
	 */
	UFUNCTION(BlueprintPure, Category = "Rules")
	bool IsTileWalkable(ATile* Tile) const;

	/**
	 * Get all available unit types from data table.
	 * @return Array of unit type IDs
	 */
	UFUNCTION(BlueprintPure, Category = "Rules")
	TArray<FName> GetAllUnitTypes() const;

protected:
	/** Cached units data table */
	UPROPERTY()
	UDataTable* UnitsDataTable;

	/** Cached tile types data table */
	UPROPERTY()
	UDataTable* TileTypesDataTable;

	/** Cached unit data for quick lookup */
	UPROPERTY()
	TMap<FName, FUnitData> CachedUnitData;

	/** Cached tile type data for quick lookup */
	UPROPERTY()
	TMap<FName, FTileTypeData> CachedTileTypeData;

	/** Cache all data from tables */
	void CacheDataFromTables();
};
