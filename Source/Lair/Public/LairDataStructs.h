// LairDataStructs.h
// Step 1: Data Structures & Enums (Foundation)
// All other files depend on these type definitions.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "LairDataStructs.generated.h"

// ============================================================================
// ENUMS
// ============================================================================

/**
 * Unit type classification for gameplay logic
 */
UENUM(BlueprintType)
enum class EUnitType : uint8
{
	Miner UMETA(DisplayName = "Miner"),
	Wagon UMETA(DisplayName = "Wagon"),
	Footman UMETA(DisplayName = "Footman"),
	Elite UMETA(DisplayName = "Elite")
};

/**
 * Army faction for unit ownership and special abilities
 */
UENUM(BlueprintType)
enum class EArmyFaction : uint8
{
	Neutral UMETA(DisplayName = "Neutral"),
	Dwarves UMETA(DisplayName = "Dwarves"),
	Humans UMETA(DisplayName = "Humans"),
	Gnomes UMETA(DisplayName = "Gnomes"),
	Orcs UMETA(DisplayName = "Orcs"),
	DarkElves UMETA(DisplayName = "Dark Elves"),
	Goblins UMETA(DisplayName = "Goblins")
};

/**
 * Turn phase for game flow control
 * Phase 1 cycles: Purchase -> Mining -> MovementCombat -> (next player's Purchase)
 */
UENUM(BlueprintType)
enum class ETurnPhase : uint8
{
	Purchase UMETA(DisplayName = "Purchase Phase"),
	Mining UMETA(DisplayName = "Mining Phase"),
	MovementCombat UMETA(DisplayName = "Movement & Combat Phase"),
	EndTurn UMETA(DisplayName = "End Turn")
};

// ============================================================================
// STRUCTS
// ============================================================================

/**
 * Unit data loaded from DT_Units data table
 * Contains all stats and properties for a unit type
 */
USTRUCT(BlueprintType)
struct FUnitData : public FTableRowBase
{
	GENERATED_BODY()

	/** Display name shown in UI */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	FText DisplayName;

	/** Gold cost to purchase this unit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	int32 Cost = 0;

	/** How far this unit can move per turn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	int32 MovementPoints = 0;

	/** Damage this unit can take before destruction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	int32 HitPoints = 1;

	/** How many sub-slots this unit occupies (1 for normal, 2 for wagons) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	int32 SubSlotSize = 1;

	/** Which faction can purchase this unit (Neutral = any faction) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	EArmyFaction AvailableToFaction = EArmyFaction::Neutral;

	/** Combat dice hit values (Phase 2+, leave empty for Phase 1) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit|Phase2")
	TArray<int32> AttackDiceValues;

	/** Number of dice this unit rolls in combat (Phase 2+, default 1) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit|Phase2")
	int32 NumberOfDice = 1;
};

/**
 * Tile type data loaded from DT_TileTypes data table
 * Defines tile properties and visual appearance
 */
USTRUCT(BlueprintType)
struct FTileTypeData : public FTableRowBase
{
	GENERATED_BODY()

	/** Display name shown in UI */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	FText DisplayName;

	/** Can units move onto this tile? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	bool bWalkable = true;

	/** Visual color for debugging in Phase 1 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	FLinearColor DebugColor = FLinearColor::White;

	/** Can miners draw mining cards here? (Phase 2+) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Phase2")
	bool bCanMine = false;

	/** Does this tile have a gate structure? (Phase 2+) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Phase2")
	bool bHasGate = false;

	/** Can units spawn here when captured? (Phase 3+) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Phase3")
	bool bIsOutpost = false;
};

/**
 * Board layout row for DT_BoardLayout data table
 * Defines the position and type of each tile on the board
 */
USTRUCT(BlueprintType)
struct FBoardLayoutRow : public FTableRowBase
{
	GENERATED_BODY()

	/** (X, Y) grid position on the board */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
	FIntPoint GridCoord = FIntPoint::ZeroValue;

	/** Row name from DT_TileTypes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
	FName TileTypeID = NAME_None;

	/** -1 if not a base, 0 for Player 1, 1 for Player 2 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
	int32 PlayerBaseIndex = -1;
};

/**
 * Faction data loaded from DT_Factions data table
 * Contains faction color, starting gold, and elite unit types
 */
USTRUCT(BlueprintType)
struct FFactionData : public FTableRowBase
{
	GENERATED_BODY()

	/** Display name shown in UI */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
	FText DisplayName;

	/** Faction color for units and UI */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
	FLinearColor FactionColor = FLinearColor::White;

	/** Gold amount at game start (200 for Phase 1) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
	int32 StartingGold = 200;

	/** Faction-specific elite unit types (Phase 3+, leave empty) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction|Phase3")
	TArray<FName> EliteUnitTypes;
};

// ============================================================================
// CONSTANTS
// ============================================================================

namespace LairConstants
{
	// Grid
	constexpr int32 DEFAULT_BOARD_SIZE_X = 10;
	constexpr int32 DEFAULT_BOARD_SIZE_Y = 10;
	constexpr int32 TILE_SUB_SLOTS = 4;
	constexpr float TILE_WORLD_SIZE = 100.0f;  // World space size of one tile in cm

	// Movement
	constexpr int32 ORTHOGONAL_MOVE_COST = 1;
	constexpr int32 DIAGONAL_MOVE_COST = 2;

	// Starting values
	constexpr int32 STARTING_GOLD = 200;

	// Player setup
	constexpr int32 MAX_PLAYERS = 2;  // Phase 1 is 2-player hotseat only
}
