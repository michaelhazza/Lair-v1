// LairGameMode.h
// Step 2: Game Mode (Central Authority)
// Owns all systems and orchestrates game flow.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LairDataStructs.h"
#include "LairGameMode.generated.h"

// Forward declarations
class UBoardSystemComponent;
class UTurnManagerComponent;
class URulesEngineComponent;
class ATile;
class AUnit;
class ALairPlayerState;

/**
 * Central game mode that owns all systems and manages game state.
 * Responsibilities:
 * - Initialize game state
 * - Own all component systems
 * - Hold references to Data Tables
 * - Start first turn
 * - Coordinate purchases and unit placement
 */
UCLASS()
class LAIR_API ALairGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALairGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void BeginPlay() override;

	// ========================================================================
	// Component References
	// ========================================================================

	/** Board system for grid management */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Systems")
	UBoardSystemComponent* BoardSystem;

	/** Turn manager for phase sequencing */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Systems")
	UTurnManagerComponent* TurnManager;

	/** Rules engine for validation logic */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Systems")
	URulesEngineComponent* RulesEngine;

	// ========================================================================
	// Data Tables
	// ========================================================================

	/** Data table containing unit stats */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	UDataTable* UnitsDataTable;

	/** Data table containing tile type definitions */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	UDataTable* TileTypesDataTable;

	/** Data table containing faction data */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	UDataTable* FactionsDataTable;

	/** Data table containing board layout */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	UDataTable* BoardLayoutDataTable;

	// ========================================================================
	// Blueprint Classes
	// ========================================================================

	/** Tile blueprint class to spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes")
	TSubclassOf<ATile> TileClass;

	/** Unit blueprint class to spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes")
	TSubclassOf<AUnit> UnitClass;

	// ========================================================================
	// Game Configuration
	// ========================================================================

	/** Number of players in this game (Phase 1: always 2) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Config")
	int32 NumberOfPlayers = 2;

	// ========================================================================
	// Public API
	// ========================================================================

	/**
	 * Start the game - initializes board and begins first turn
	 */
	UFUNCTION(BlueprintCallable, Category = "Game")
	void StartGame();

	/**
	 * Purchase a unit for the specified player
	 * @param PlayerIndex - The player making the purchase (0 or 1)
	 * @param UnitTypeID - Row name from DT_Units (e.g., "Footman")
	 * @return True if purchase succeeded, false if failed
	 */
	UFUNCTION(BlueprintCallable, Category = "Game")
	bool PurchaseUnit(int32 PlayerIndex, FName UnitTypeID);

	/**
	 * Get the player state for a specific player index
	 * @param PlayerIndex - The player index (0 or 1)
	 * @return Player state or nullptr if not found
	 */
	UFUNCTION(BlueprintPure, Category = "Game")
	ALairPlayerState* GetPlayerState(int32 PlayerIndex) const;

	/**
	 * Get the board system component
	 * @return BoardSystemComponent pointer
	 */
	UFUNCTION(BlueprintPure, Category = "Game")
	UBoardSystemComponent* GetBoardSystem() const { return BoardSystem; }

	/**
	 * Get the turn manager component
	 * @return TurnManagerComponent pointer
	 */
	UFUNCTION(BlueprintPure, Category = "Game")
	UTurnManagerComponent* GetTurnManager() const { return TurnManager; }

	/**
	 * Get the rules engine component
	 * @return RulesEngineComponent pointer
	 */
	UFUNCTION(BlueprintPure, Category = "Game")
	URulesEngineComponent* GetRulesEngine() const { return RulesEngine; }

	/**
	 * End the current player's turn
	 */
	UFUNCTION(BlueprintCallable, Category = "Game")
	void EndCurrentTurn();

protected:
	/** Cached player states */
	UPROPERTY()
	TArray<ALairPlayerState*> PlayerStates;

	/** Spawn a unit at the player's base */
	AUnit* SpawnUnitAtBase(int32 PlayerIndex, FName UnitTypeID);
};
