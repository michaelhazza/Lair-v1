// LairPlayerState.cpp
// Step 5: Player State (Per-Player Data)

#include "LairPlayerState.h"
#include "Unit.h"

ALairPlayerState::ALairPlayerState()
{
	Gold = 0;
	PlayerIndex = 0;
	Faction = EArmyFaction::Neutral;
}

void ALairPlayerState::InitializePlayer(int32 InPlayerIndex, int32 StartingGold)
{
	PlayerIndex = InPlayerIndex;
	Gold = StartingGold;

	// Set default faction based on player index (Phase 1: generic colors)
	Faction = EArmyFaction::Neutral;

	UE_LOG(LogTemp, Log, TEXT("ALairPlayerState::InitializePlayer - Player %d initialized with %d gold"),
		PlayerIndex, Gold);
}

void ALairPlayerState::AddGold(int32 Amount)
{
	if (Amount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ALairPlayerState::AddGold - Invalid amount %d"), Amount);
		return;
	}

	int32 OldGold = Gold;
	Gold += Amount;

	UE_LOG(LogTemp, Log, TEXT("ALairPlayerState::AddGold - Player %d: %d + %d = %d gold"),
		PlayerIndex, OldGold, Amount, Gold);

	OnGoldChanged.Broadcast(OldGold, Gold);
}

void ALairPlayerState::DeductGold(int32 Amount)
{
	if (Amount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ALairPlayerState::DeductGold - Invalid amount %d"), Amount);
		return;
	}

	if (Amount > Gold)
	{
		UE_LOG(LogTemp, Warning, TEXT("ALairPlayerState::DeductGold - Cannot deduct %d from %d gold"),
			Amount, Gold);
		return;
	}

	int32 OldGold = Gold;
	Gold -= Amount;

	UE_LOG(LogTemp, Log, TEXT("ALairPlayerState::DeductGold - Player %d: %d - %d = %d gold"),
		PlayerIndex, OldGold, Amount, Gold);

	OnGoldChanged.Broadcast(OldGold, Gold);
}

bool ALairPlayerState::CanAfford(int32 Cost) const
{
	return Gold >= Cost;
}

void ALairPlayerState::SetGold(int32 Amount)
{
	int32 OldGold = Gold;
	Gold = FMath::Max(0, Amount);

	if (OldGold != Gold)
	{
		UE_LOG(LogTemp, Log, TEXT("ALairPlayerState::SetGold - Player %d: gold set to %d"),
			PlayerIndex, Gold);

		OnGoldChanged.Broadcast(OldGold, Gold);
	}
}

void ALairPlayerState::SetFaction(EArmyFaction NewFaction)
{
	Faction = NewFaction;
	UE_LOG(LogTemp, Log, TEXT("ALairPlayerState::SetFaction - Player %d: faction set to %d"),
		PlayerIndex, static_cast<int32>(Faction));
}

void ALairPlayerState::AddOwnedUnit(AUnit* Unit)
{
	if (Unit && !OwnedUnits.Contains(Unit))
	{
		OwnedUnits.Add(Unit);
		UE_LOG(LogTemp, Verbose, TEXT("ALairPlayerState::AddOwnedUnit - Player %d now owns %d units"),
			PlayerIndex, OwnedUnits.Num());
	}
}

void ALairPlayerState::RemoveOwnedUnit(AUnit* Unit)
{
	if (Unit)
	{
		OwnedUnits.Remove(Unit);
		UE_LOG(LogTemp, Verbose, TEXT("ALairPlayerState::RemoveOwnedUnit - Player %d now owns %d units"),
			PlayerIndex, OwnedUnits.Num());
	}
}
