// TurnManagerComponent.h
// Step 7: Turn Manager Component (Phase Sequencing)
// Manages turn phases and player rotation.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LairDataStructs.h"
#include "TurnManagerComponent.generated.h"

/** Delegate for phase changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, ETurnPhase, NewPhase);

/** Delegate for player changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerChanged, int32, NewPlayerIndex);

/** Delegate for turn number changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurnChanged, int32, NewTurnNumber);

/**
 * Component that manages turn phases and player rotation.
 * Responsibilities:
 * - Track current turn phase
 * - Track current player index
 * - Advance phases and players
 * - Broadcast phase/player change events
 * - Count total turns
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAIR_API UTurnManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTurnManagerComponent();

	// ========================================================================
	// Stable API - DO NOT MODIFY SIGNATURES
	// ========================================================================

	/**
	 * Start the first turn of the game.
	 */
	UFUNCTION(BlueprintCallable, Category = "Turn")
	void StartFirstTurn();

	/**
	 * Advance to the next phase.
	 * After MovementCombat, advances to next player's Purchase phase.
	 */
	UFUNCTION(BlueprintCallable, Category = "Turn")
	void AdvancePhase();

	/**
	 * End the current player's turn completely.
	 * Advances to next player's Purchase phase.
	 */
	UFUNCTION(BlueprintCallable, Category = "Turn")
	void EndTurn();

	/**
	 * Get the current turn phase.
	 * @return Current phase
	 */
	UFUNCTION(BlueprintPure, Category = "Turn")
	ETurnPhase GetCurrentPhase() const { return CurrentPhase; }

	/**
	 * Get the current player index.
	 * @return Current player index (0 or 1)
	 */
	UFUNCTION(BlueprintPure, Category = "Turn")
	int32 GetCurrentPlayerIndex() const { return CurrentPlayerIndex; }

	// ========================================================================
	// Additional API
	// ========================================================================

	/**
	 * Get the current turn number.
	 * @return Turn number (starts at 1)
	 */
	UFUNCTION(BlueprintPure, Category = "Turn")
	int32 GetTurnNumber() const { return TurnNumber; }

	/**
	 * Set the total number of players.
	 * @param InTotalPlayers - Number of players
	 */
	void SetTotalPlayers(int32 InTotalPlayers) { TotalPlayers = InTotalPlayers; }

	/**
	 * Get the total number of players.
	 * @return Total player count
	 */
	UFUNCTION(BlueprintPure, Category = "Turn")
	int32 GetTotalPlayers() const { return TotalPlayers; }

	// ========================================================================
	// Events
	// ========================================================================

	/** Broadcast when phase changes */
	UPROPERTY(BlueprintAssignable, Category = "Turn")
	FOnPhaseChanged OnPhaseChanged;

	/** Broadcast when current player changes */
	UPROPERTY(BlueprintAssignable, Category = "Turn")
	FOnPlayerChanged OnPlayerChanged;

	/** Broadcast when turn number changes */
	UPROPERTY(BlueprintAssignable, Category = "Turn")
	FOnTurnChanged OnTurnChanged;

protected:
	/** Current turn phase */
	UPROPERTY(BlueprintReadOnly, Category = "Turn")
	ETurnPhase CurrentPhase;

	/** Current player index */
	UPROPERTY(BlueprintReadOnly, Category = "Turn")
	int32 CurrentPlayerIndex;

	/** Current turn number */
	UPROPERTY(BlueprintReadOnly, Category = "Turn")
	int32 TurnNumber;

	/** Total number of players */
	UPROPERTY(BlueprintReadOnly, Category = "Turn")
	int32 TotalPlayers;

	/** Set the current phase and broadcast event */
	void SetPhase(ETurnPhase NewPhase);

	/** Advance to the next player */
	void AdvancePlayer();
};
