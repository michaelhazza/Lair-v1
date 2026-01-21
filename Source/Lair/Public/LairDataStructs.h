// LairDataStructs.h
// LAIR - Phase 1 Foundation
// All enums and structs for the game. No dependencies on other LAIR files.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "LairDataStructs.generated.h"

// ============================================================================
// ENUMS
// ============================================================================

/**
 * Represents the type of unit.
 * Phase 1: Miner, Wagon, Footman only.
 * Phase 3+: Elite units added per faction.
 */
UENUM(BlueprintType)
enum class EUnitType : uint8
{
	None        UMETA(DisplayName = "None"),
	Miner       UMETA(DisplayName = "Miner"),
	Wagon       UMETA(DisplayName = "Wagon"),
	Footman     UMETA(DisplayName = "Footman"),
	// Phase 3+ elite units will be added here
};

/**
 * Represents the army faction/player identity.
 * Phase 1: Generic Player1/Player2.
 * Phase 3+: Dwarves, Humans, etc. with special abilities.
 */
UENUM(BlueprintType)
enum class EArmyFaction : uint8
{
	None        UMETA(DisplayName = "None"),
	Any         UMETA(DisplayName = "Any"),       // Available to all factions
	Player1     UMETA(DisplayName = "Player 1"),  // Phase 1 generic
	Player2     UMETA(DisplayName = "Player 2"),  // Phase 1 generic
	// Phase 3+: Dwarves, Humans, Orcs, Elves, etc.
};

/**
 * Represents the current phase within a player's turn.
 * Turn order: Purchase -> Mining -> Movement -> (Next Player)
 */
UENUM(BlueprintType)
enum class ETurnPhase : uint8
{
	None        UMETA(DisplayName = "None"),
	Purchase    UMETA(DisplayName = "Purchase Phase"),
	Mining      UMETA(DisplayName = "Mining Phase"),
	Movement    UMETA(DisplayName = "Movement Phase"),
	// Phase 2+: Combat phase added after Movement
};

/**
 * Represents the type of tile on the board.
 * Phase 1: Empty and PlayerBase only.
 * Phase 2+: Mining, Gate, Mystery tiles added.
 */
UENUM(BlueprintType)
enum class ETileType : uint8
{
	None        UMETA(DisplayName = "None"),
	Empty       UMETA(DisplayName = "Empty"),
	PlayerBase  UMETA(DisplayName = "Player Base"),
	// Phase 2+: Mining, Gate, Mystery
	// Phase 3+: Outpost
};

// ============================================================================
// STRUCTS - Data Table Row Types
// ============================================================================

/**
 * Unit data loaded from DT_Units data table.
 * Contains all stats for a purchasable unit type.
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

	/** How many tiles this unit can move per turn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	int32 MovementPoints = 0;

	/** Damage this unit can take before destruction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	int32 HitPoints = 1;

	/** Sub-slots required on a tile (1 for normal units, 2 for wagons) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	int32 SubSlotSize = 1;

	/** Which faction can purchase this unit (Any = all factions) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	EArmyFaction AvailableToFaction = EArmyFaction::Any;

	/** Combat dice values (Phase 2+, leave empty for Phase 1) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit|Phase2")
	TArray<int32> AttackDiceValues;

	/** Number of dice to roll in combat (Phase 2+) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit|Phase2")
	int32 NumberOfDice = 1;

	FUnitData()
		: Cost(0)
		, MovementPoints(0)
		, HitPoints(1)
		, SubSlotSize(1)
		, AvailableToFaction(EArmyFaction::Any)
		, NumberOfDice(1)
	{
	}
};

/**
 * Tile type data loaded from DT_TileTypes data table.
 * Defines properties for each tile category.
 */
USTRUCT(BlueprintType)
struct FTileTypeData : public FTableRowBase
{
	GENERATED_BODY()

	/** Display name for this tile type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	FText DisplayName;

	/** Can units move onto this tile? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	bool bWalkable = true;

	/** Visual color for Phase 1 debug display */
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

	FTileTypeData()
		: bWalkable(true)
		, DebugColor(FLinearColor::White)
		, bCanMine(false)
		, bHasGate(false)
		, bIsOutpost(false)
	{
	}
};

/**
 * Faction data loaded from DT_Factions data table.
 * Defines properties for each playable faction.
 */
USTRUCT(BlueprintType)
struct FFactionData : public FTableRowBase
{
	GENERATED_BODY()

	/** Display name for this faction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
	FText DisplayName;

	/** Color tint applied to faction's units */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
	FLinearColor FactionColor = FLinearColor::White;

	/** Starting gold for this faction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
	int32 StartingGold = 200;

	/** Faction-specific elite unit types (Phase 3+, leave empty) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction|Phase3")
	TArray<FName> EliteUnitTypes;

	/** Description of faction's special ability (Phase 3+) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction|Phase3")
	FText FactionAbilityDescription;

	FFactionData()
		: FactionColor(FLinearColor::White)
		, StartingGold(200)
	{
	}
};

/**
 * Board layout row loaded from DT_BoardLayout_* data tables.
 * Defines the grid position and type of each tile on the board.
 */
USTRUCT(BlueprintType)
struct FBoardLayoutRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Grid coordinate (X, Y) for this tile */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
	FIntPoint GridCoord = FIntPoint::ZeroValue;

	/** Row name from DT_TileTypes identifying the tile type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
	FName TileTypeID = NAME_None;

	/** Player base index: -1 if not a base, 0 for P1, 1 for P2 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
	int32 PlayerBaseIndex = -1;

	FBoardLayoutRow()
		: GridCoord(FIntPoint::ZeroValue)
		, TileTypeID(NAME_None)
		, PlayerBaseIndex(-1)
	{
	}
};

// ============================================================================
// DELEGATES
// ============================================================================

/** Broadcast when a turn phase changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, ETurnPhase, NewPhase);

/** Broadcast when the active player changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerChanged, int32, NewPlayerIndex);

/** Broadcast when a player's gold changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGoldChanged, int32, PlayerIndex, int32, NewGoldAmount);

/** Broadcast when a unit is spawned */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitSpawned, class AUnit*, Unit, class ATile*, SpawnTile);

/** Broadcast when a unit is removed from play */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitRemoved, class AUnit*, Unit);
