// TurnManagerComponent.cpp
// Step 7: Turn Manager Component (Phase Sequencing)

#include "TurnManagerComponent.h"

UTurnManagerComponent::UTurnManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CurrentPhase = ETurnPhase::Purchase;
	CurrentPlayerIndex = 0;
	TurnNumber = 0;
	TotalPlayers = 2; // Default for Phase 1
}

void UTurnManagerComponent::StartFirstTurn()
{
	CurrentPlayerIndex = 0;
	TurnNumber = 1;
	SetPhase(ETurnPhase::Purchase);

	UE_LOG(LogTemp, Log, TEXT("UTurnManagerComponent::StartFirstTurn - Turn %d, Player %d, Phase: Purchase"),
		TurnNumber, CurrentPlayerIndex);

	// Broadcast initial state
	OnPlayerChanged.Broadcast(CurrentPlayerIndex);
	OnTurnChanged.Broadcast(TurnNumber);
}

void UTurnManagerComponent::AdvancePhase()
{
	ETurnPhase OldPhase = CurrentPhase;

	switch (CurrentPhase)
	{
	case ETurnPhase::Purchase:
		SetPhase(ETurnPhase::Mining);
		break;

	case ETurnPhase::Mining:
		SetPhase(ETurnPhase::MovementCombat);
		break;

	case ETurnPhase::MovementCombat:
		// End of this player's turn - advance to next player
		AdvancePlayer();
		SetPhase(ETurnPhase::Purchase);
		break;

	case ETurnPhase::EndTurn:
		// EndTurn is a transient state, go to next player's Purchase
		AdvancePlayer();
		SetPhase(ETurnPhase::Purchase);
		break;

	default:
		SetPhase(ETurnPhase::Purchase);
		break;
	}

	UE_LOG(LogTemp, Log, TEXT("UTurnManagerComponent::AdvancePhase - Phase changed from %d to %d"),
		static_cast<int32>(OldPhase), static_cast<int32>(CurrentPhase));
}

void UTurnManagerComponent::EndTurn()
{
	UE_LOG(LogTemp, Log, TEXT("UTurnManagerComponent::EndTurn - Player %d ending turn"), CurrentPlayerIndex);

	// Advance to next player
	AdvancePlayer();

	// Start next player's Purchase phase
	SetPhase(ETurnPhase::Purchase);
}

void UTurnManagerComponent::SetPhase(ETurnPhase NewPhase)
{
	ETurnPhase OldPhase = CurrentPhase;
	CurrentPhase = NewPhase;

	UE_LOG(LogTemp, Verbose, TEXT("UTurnManagerComponent::SetPhase - Phase: %d -> %d"),
		static_cast<int32>(OldPhase), static_cast<int32>(CurrentPhase));

	OnPhaseChanged.Broadcast(CurrentPhase);
}

void UTurnManagerComponent::AdvancePlayer()
{
	int32 OldPlayerIndex = CurrentPlayerIndex;

	// Move to next player
	CurrentPlayerIndex = (CurrentPlayerIndex + 1) % TotalPlayers;

	// If we wrapped around to player 0, increment turn number
	if (CurrentPlayerIndex == 0)
	{
		TurnNumber++;
		OnTurnChanged.Broadcast(TurnNumber);
		UE_LOG(LogTemp, Log, TEXT("UTurnManagerComponent::AdvancePlayer - New turn %d"), TurnNumber);
	}

	UE_LOG(LogTemp, Log, TEXT("UTurnManagerComponent::AdvancePlayer - Player %d -> %d"),
		OldPlayerIndex, CurrentPlayerIndex);

	OnPlayerChanged.Broadcast(CurrentPlayerIndex);
}
