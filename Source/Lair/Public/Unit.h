// Unit.h
// Step 4: Unit Actor (Game Pieces)
// Represents game pieces (Miner, Footman, Wagon).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LairDataStructs.h"
#include "Unit.generated.h"

// Forward declaration
class ATile;

/**
 * Represents a game piece (unit) on the board.
 * Responsibilities:
 * - Represent game pieces (Miner, Footman, Wagon)
 * - Load stats from Data Table
 * - Track current tile and sub-slot position
 * - Visual mesh representation
 */
UCLASS()
class LAIR_API AUnit : public AActor
{
	GENERATED_BODY()

public:
	AUnit();

	virtual void BeginPlay() override;

	// ========================================================================
	// Properties
	// ========================================================================

	/** Unit type ID (row name from DT_Units) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	FName UnitTypeID;

	/** Cached unit data from data table */
	UPROPERTY(BlueprintReadOnly, Category = "Unit")
	FUnitData CachedUnitData;

	/** Owner player index (0 or 1) */
	UPROPERTY(BlueprintReadWrite, Category = "Unit")
	int32 OwnerPlayerIndex = 0;

	/** Current tile this unit is on */
	UPROPERTY(BlueprintReadWrite, Category = "Unit")
	ATile* CurrentTile = nullptr;

	/** Sub-slot index on current tile (0-3) */
	UPROPERTY(BlueprintReadWrite, Category = "Unit")
	int32 SubSlotIndex = 0;

	/** Remaining movement points this turn */
	UPROPERTY(BlueprintReadWrite, Category = "Unit")
	int32 RemainingMovement = 0;

	/** Current HP */
	UPROPERTY(BlueprintReadWrite, Category = "Unit")
	int32 CurrentHP = 1;

	// ========================================================================
	// Visual Components
	// ========================================================================

	/** Root component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootSceneComponent;

	/** Unit mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* UnitMesh;

	// ========================================================================
	// Stable API - DO NOT MODIFY SIGNATURES
	// ========================================================================

	/**
	 * Initialize unit from data table entry.
	 * @param InUnitTypeID - Row name from DT_Units
	 * @param Data - Unit data struct
	 */
	UFUNCTION(BlueprintCallable, Category = "Unit")
	void InitializeFromDataTable(FName InUnitTypeID, const FUnitData& Data);

	/**
	 * Reset movement points to maximum for a new turn.
	 */
	UFUNCTION(BlueprintCallable, Category = "Unit")
	void ResetMovement();

	/**
	 * Set the current tile and sub-slot for this unit.
	 * @param NewTile - The tile to place unit on
	 * @param InSubSlotIndex - The sub-slot index (0-3)
	 */
	UFUNCTION(BlueprintCallable, Category = "Unit")
	void SetCurrentTile(ATile* NewTile, int32 InSubSlotIndex);

	/**
	 * Check if this unit is on the specified tile.
	 * @param Tile - Tile to check
	 * @return True if unit is on this tile
	 */
	UFUNCTION(BlueprintPure, Category = "Unit")
	bool IsAtTile(ATile* Tile) const;

	// ========================================================================
	// Additional API
	// ========================================================================

	/**
	 * Get the remaining movement points.
	 * @return Remaining movement points
	 */
	UFUNCTION(BlueprintPure, Category = "Unit")
	int32 GetRemainingMovement() const { return RemainingMovement; }

	/**
	 * Get the sub-slot size for this unit.
	 * @return Sub-slot size (1 for normal, 2 for wagons)
	 */
	UFUNCTION(BlueprintPure, Category = "Unit")
	int32 GetSubSlotSize() const { return CachedUnitData.SubSlotSize; }

	/**
	 * Set selection highlight.
	 * @param bSelected - Whether unit is selected
	 */
	UFUNCTION(BlueprintCallable, Category = "Unit")
	void SetSelected(bool bSelected);

	/**
	 * Set unit color based on owner.
	 * @param Color - Color to apply
	 */
	UFUNCTION(BlueprintCallable, Category = "Unit")
	void SetUnitColor(FLinearColor Color);

protected:
	/** Dynamic material instance for color changes */
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	/** Update visual representation */
	void UpdateVisuals();
};
