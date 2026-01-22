// LairPlayerController.h
// Step 9: Player Controller (Input Handling)
// Handles player input and communicates with game mode.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LairDataStructs.h"
#include "LairPlayerController.generated.h"

// Forward declarations
class ATile;
class AUnit;
class ALairGameMode;

/** Delegate for tile selection */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileSelected, ATile*, SelectedTile);

/** Delegate for unit selection */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitSelected, AUnit*, SelectedUnit);

/**
 * Player controller for LAIR game.
 * Responsibilities:
 * - Detect mouse clicks on tiles
 * - Detect UI button clicks
 * - Convert input to game actions
 * - Communicate with GameMode to execute actions
 * - Enable mouse cursor
 */
UCLASS()
class LAIR_API ALairPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ALairPlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// ========================================================================
	// Selection State
	// ========================================================================

	/** Currently selected tile */
	UPROPERTY(BlueprintReadOnly, Category = "Selection")
	ATile* SelectedTile;

	/** Currently selected unit */
	UPROPERTY(BlueprintReadOnly, Category = "Selection")
	AUnit* SelectedUnit;

	// ========================================================================
	// Events
	// ========================================================================

	/** Broadcast when a tile is selected */
	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnTileSelected OnTileSelected;

	/** Broadcast when a unit is selected */
	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnUnitSelected OnUnitSelected;

	// ========================================================================
	// Input Handlers
	// ========================================================================

	/**
	 * Handle left mouse button click.
	 */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnLeftMouseClick();

	/**
	 * Handle right mouse button click.
	 */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnRightMouseClick();

	// ========================================================================
	// UI Actions
	// ========================================================================

	/**
	 * Request to purchase a unit.
	 * @param UnitTypeID - Row name from DT_Units
	 */
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void OnPurchaseButtonClicked(FName UnitTypeID);

	/**
	 * Request to end the current turn.
	 */
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void OnEndTurnClicked();

	/**
	 * Request to advance to the next phase.
	 */
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void OnAdvancePhaseClicked();

	// ========================================================================
	// Queries
	// ========================================================================

	/**
	 * Get the tile under the mouse cursor.
	 * @return Tile under cursor or nullptr
	 */
	UFUNCTION(BlueprintPure, Category = "Input")
	ATile* GetTileUnderCursor() const;

	/**
	 * Get the unit under the mouse cursor.
	 * @return Unit under cursor or nullptr
	 */
	UFUNCTION(BlueprintPure, Category = "Input")
	AUnit* GetUnitUnderCursor() const;

	/**
	 * Get the player index this controller represents.
	 * For hotseat mode, returns the current player from TurnManager.
	 * @return Player index
	 */
	UFUNCTION(BlueprintPure, Category = "Player")
	int32 GetLocalPlayerIndex() const;

protected:
	/** Cached game mode reference */
	UPROPERTY()
	ALairGameMode* GameModeRef;

	/** Select a tile */
	void SelectTile(ATile* Tile);

	/** Select a unit */
	void SelectUnit(AUnit* Unit);

	/** Clear selection */
	void ClearSelection();
};
