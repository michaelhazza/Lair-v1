// LairPlayerState.h
// Step 5: Player State (Per-Player Data)
// Tracks gold, faction, and units for each player.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LairDataStructs.h"
#include "LairPlayerState.generated.h"

// Forward declaration
class AUnit;

/** Delegate for gold changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGoldChanged, int32, OldGold, int32, NewGold);

/**
 * Per-player state for LAIR game.
 * Responsibilities:
 * - Track player gold
 * - Track player faction
 * - Track units owned by player
 * - Expose gold modification functions
 */
UCLASS()
class LAIR_API ALairPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ALairPlayerState();

	// ========================================================================
	// Properties
	// ========================================================================

	/** Player index (0 or 1) */
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	int32 PlayerIndex = 0;

	/** Player's faction */
	UPROPERTY(BlueprintReadWrite, Category = "Player")
	EArmyFaction Faction = EArmyFaction::Neutral;

	// ========================================================================
	// Events
	// ========================================================================

	/** Broadcast when gold amount changes */
	UPROPERTY(BlueprintAssignable, Category = "Player")
	FOnGoldChanged OnGoldChanged;

	// ========================================================================
	// Gold Management API
	// ========================================================================

	/**
	 * Get current gold amount.
	 * @return Current gold
	 */
	UFUNCTION(BlueprintPure, Category = "Player")
	int32 GetGold() const { return Gold; }

	/**
	 * Add gold to player.
	 * @param Amount - Amount to add
	 */
	UFUNCTION(BlueprintCallable, Category = "Player")
	void AddGold(int32 Amount);

	/**
	 * Deduct gold from player.
	 * @param Amount - Amount to deduct
	 */
	UFUNCTION(BlueprintCallable, Category = "Player")
	void DeductGold(int32 Amount);

	/**
	 * Check if player can afford a cost.
	 * @param Cost - Amount to check
	 * @return True if gold >= cost
	 */
	UFUNCTION(BlueprintPure, Category = "Player")
	bool CanAfford(int32 Cost) const;

	/**
	 * Set gold directly (for testing/initialization).
	 * @param Amount - New gold amount
	 */
	UFUNCTION(BlueprintCallable, Category = "Player")
	void SetGold(int32 Amount);

	// ========================================================================
	// Faction API
	// ========================================================================

	/**
	 * Get player's faction.
	 * @return Current faction
	 */
	UFUNCTION(BlueprintPure, Category = "Player")
	EArmyFaction GetFaction() const { return Faction; }

	/**
	 * Set player's faction.
	 * @param NewFaction - Faction to set
	 */
	UFUNCTION(BlueprintCallable, Category = "Player")
	void SetFaction(EArmyFaction NewFaction);

	// ========================================================================
	// Unit Tracking
	// ========================================================================

	/**
	 * Get all units owned by this player.
	 * @return Array of owned units
	 */
	UFUNCTION(BlueprintPure, Category = "Player")
	TArray<AUnit*> GetOwnedUnits() const { return OwnedUnits; }

	/**
	 * Add a unit to this player's ownership.
	 * @param Unit - Unit to add
	 */
	UFUNCTION(BlueprintCallable, Category = "Player")
	void AddOwnedUnit(AUnit* Unit);

	/**
	 * Remove a unit from this player's ownership.
	 * @param Unit - Unit to remove
	 */
	UFUNCTION(BlueprintCallable, Category = "Player")
	void RemoveOwnedUnit(AUnit* Unit);

	// ========================================================================
	// Initialization
	// ========================================================================

	/**
	 * Initialize player state.
	 * @param InPlayerIndex - Player index (0 or 1)
	 * @param StartingGold - Initial gold amount
	 */
	UFUNCTION(BlueprintCallable, Category = "Player")
	void InitializePlayer(int32 InPlayerIndex, int32 StartingGold);

protected:
	/** Current gold amount */
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	int32 Gold = 0;

	/** Units owned by this player */
	UPROPERTY()
	TArray<AUnit*> OwnedUnits;
};
