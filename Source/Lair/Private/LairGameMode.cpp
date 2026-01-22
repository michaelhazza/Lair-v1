// LairGameMode.cpp
// Step 2: Game Mode (Central Authority)

#include "LairGameMode.h"
#include "BoardSystemComponent.h"
#include "TurnManagerComponent.h"
#include "RulesEngineComponent.h"
#include "LairPlayerState.h"
#include "Tile.h"
#include "Unit.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

ALairGameMode::ALairGameMode()
{
	// Create component systems
	BoardSystem = CreateDefaultSubobject<UBoardSystemComponent>(TEXT("BoardSystem"));
	TurnManager = CreateDefaultSubobject<UTurnManagerComponent>(TEXT("TurnManager"));
	RulesEngine = CreateDefaultSubobject<URulesEngineComponent>(TEXT("RulesEngine"));

	// Default player state class
	PlayerStateClass = ALairPlayerState::StaticClass();

	// Phase 1: 2 players
	NumberOfPlayers = 2;
}

void ALairGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	UE_LOG(LogTemp, Log, TEXT("ALairGameMode::InitGame - Initializing LAIR game"));
}

void ALairGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("ALairGameMode::BeginPlay - Starting game setup"));

	// Initialize the rules engine with data tables
	if (RulesEngine && UnitsDataTable && TileTypesDataTable)
	{
		RulesEngine->Initialize(UnitsDataTable, TileTypesDataTable);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ALairGameMode::BeginPlay - Missing data tables or RulesEngine"));
	}

	// Start the game
	StartGame();
}

void ALairGameMode::StartGame()
{
	UE_LOG(LogTemp, Log, TEXT("ALairGameMode::StartGame - Initializing game"));

	// Initialize the board
	if (BoardSystem)
	{
		// Pass required references to BoardSystem
		BoardSystem->SetTileClass(TileClass);
		BoardSystem->SetBoardLayoutDataTable(BoardLayoutDataTable);
		BoardSystem->SetTileTypesDataTable(TileTypesDataTable);

		// Initialize board from data table path (or generate default 10x10)
		BoardSystem->InitializeBoard(TEXT(""));
	}

	// Cache player states
	PlayerStates.Empty();
	AGameStateBase* GS = GetGameState<AGameStateBase>();
	for (int32 i = 0; i < NumberOfPlayers; ++i)
	{
		// Create player states directly for hotseat mode
		ALairPlayerState* NewPlayerState = GetWorld()->SpawnActor<ALairPlayerState>();
		if (NewPlayerState)
		{
			NewPlayerState->InitializePlayer(i, LairConstants::STARTING_GOLD);
			PlayerStates.Add(NewPlayerState);

			// Register with GameState so engine systems (replication, UI, analytics) can see these states
			if (GS)
			{
				GS->AddPlayerState(NewPlayerState);
			}

			UE_LOG(LogTemp, Log, TEXT("Created PlayerState for Player %d with %d gold"),
				i, LairConstants::STARTING_GOLD);
		}
	}

	// Start the first turn
	if (TurnManager)
	{
		TurnManager->SetTotalPlayers(NumberOfPlayers);
		TurnManager->StartFirstTurn();
	}

	UE_LOG(LogTemp, Log, TEXT("ALairGameMode::StartGame - Game started with %d players"), NumberOfPlayers);
}

bool ALairGameMode::PurchaseUnit(int32 PlayerIndex, FName UnitTypeID)
{
	// Validate player index
	if (PlayerIndex < 0 || PlayerIndex >= PlayerStates.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("PurchaseUnit: Invalid player index %d"), PlayerIndex);
		return false;
	}

	// Check if it's the correct player's turn
	if (TurnManager && TurnManager->GetCurrentPlayerIndex() != PlayerIndex)
	{
		UE_LOG(LogTemp, Warning, TEXT("PurchaseUnit: Not player %d's turn"), PlayerIndex);
		return false;
	}

	// Check if we're in the purchase phase
	if (TurnManager && TurnManager->GetCurrentPhase() != ETurnPhase::Purchase)
	{
		UE_LOG(LogTemp, Warning, TEXT("PurchaseUnit: Not in Purchase phase"));
		return false;
	}

	ALairPlayerState* PlayerState = PlayerStates[PlayerIndex];
	if (!PlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("PurchaseUnit: PlayerState is null"));
		return false;
	}

	// Get unit data from rules engine
	if (!RulesEngine)
	{
		UE_LOG(LogTemp, Warning, TEXT("PurchaseUnit: RulesEngine is null"));
		return false;
	}

	// Check if player can afford the unit
	int32 PlayerGold = PlayerState->GetGold();
	if (!RulesEngine->CanAffordUnit(PlayerGold, UnitTypeID))
	{
		UE_LOG(LogTemp, Warning, TEXT("PurchaseUnit: Player %d cannot afford unit %s (has %d gold)"),
			PlayerIndex, *UnitTypeID.ToString(), PlayerGold);
		return false;
	}

	// Get the player's base tile
	if (!BoardSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("PurchaseUnit: BoardSystem is null"));
		return false;
	}

	FIntPoint BaseCoord = BoardSystem->GetPlayerBaseCoord(PlayerIndex);
	ATile* BaseTile = BoardSystem->GetTileAt(BaseCoord);
	if (!BaseTile)
	{
		UE_LOG(LogTemp, Warning, TEXT("PurchaseUnit: Could not find base tile at (%d, %d)"),
			BaseCoord.X, BaseCoord.Y);
		return false;
	}

	// Get unit data for sub-slot size check
	FUnitData UnitData = RulesEngine->GetUnitData(UnitTypeID);

	// Check if base tile has room
	if (!RulesEngine->CanSpawnUnitAt(BaseTile, UnitData.SubSlotSize))
	{
		UE_LOG(LogTemp, Warning, TEXT("PurchaseUnit: Not enough space at base for unit %s"),
			*UnitTypeID.ToString());
		return false;
	}

	// Deduct gold
	PlayerState->DeductGold(UnitData.Cost);

	// Spawn the unit
	AUnit* NewUnit = SpawnUnitAtBase(PlayerIndex, UnitTypeID);
	if (!NewUnit)
	{
		// Refund gold if spawn failed
		PlayerState->AddGold(UnitData.Cost);
		UE_LOG(LogTemp, Warning, TEXT("PurchaseUnit: Failed to spawn unit, gold refunded"));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("PurchaseUnit: Player %d purchased %s for %d gold (remaining: %d)"),
		PlayerIndex, *UnitTypeID.ToString(), UnitData.Cost, PlayerState->GetGold());

	return true;
}

AUnit* ALairGameMode::SpawnUnitAtBase(int32 PlayerIndex, FName UnitTypeID)
{
	if (!UnitClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnUnitAtBase: UnitClass is not set"));
		return nullptr;
	}

	if (!BoardSystem)
	{
		return nullptr;
	}

	FIntPoint BaseCoord = BoardSystem->GetPlayerBaseCoord(PlayerIndex);
	ATile* BaseTile = BoardSystem->GetTileAt(BaseCoord);
	if (!BaseTile)
	{
		return nullptr;
	}

	// Get unit data
	FUnitData UnitData = RulesEngine->GetUnitData(UnitTypeID);

	// Find an available sub-slot
	int32 AvailableSubSlot = -1;
	for (int32 i = 0; i < LairConstants::TILE_SUB_SLOTS; ++i)
	{
		// For wagons (size 2), need contiguous slots
		if (UnitData.SubSlotSize == 2)
		{
			// Check if both slot i and i+1 are available
			if (i < LairConstants::TILE_SUB_SLOTS - 1)
			{
				TArray<AUnit*> UnitsOnTile = BaseTile->GetAllUnitsOnTile();
				bool Slot1Free = true;
				bool Slot2Free = true;
				for (AUnit* Unit : UnitsOnTile)
				{
					if (Unit && Unit->SubSlotIndex == i) Slot1Free = false;
					if (Unit && Unit->SubSlotIndex == i + 1) Slot2Free = false;
				}
				if (Slot1Free && Slot2Free)
				{
					AvailableSubSlot = i;
					break;
				}
			}
		}
		else
		{
			// Size 1 unit - find any free slot
			TArray<AUnit*> UnitsOnTile = BaseTile->GetAllUnitsOnTile();
			bool SlotFree = true;
			for (AUnit* Unit : UnitsOnTile)
			{
				if (Unit && Unit->SubSlotIndex == i)
				{
					SlotFree = false;
					break;
				}
			}
			if (SlotFree)
			{
				AvailableSubSlot = i;
				break;
			}
		}
	}

	if (AvailableSubSlot < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnUnitAtBase: No available sub-slot"));
		return nullptr;
	}

	// Calculate spawn location
	FVector SpawnLocation = BaseTile->GetActorLocation();
	SpawnLocation.Z += 50.0f; // Raise unit above tile

	// Spawn the unit
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AUnit* NewUnit = GetWorld()->SpawnActor<AUnit>(UnitClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	if (NewUnit)
	{
		// Set owner BEFORE initializing from data table, so UpdateVisuals() uses correct player color
		NewUnit->OwnerPlayerIndex = PlayerIndex;

		// Initialize the unit (this calls UpdateVisuals internally)
		NewUnit->InitializeFromDataTable(UnitTypeID, UnitData);

		// Place on tile
		BaseTile->PlaceUnitInSubSlot(NewUnit, AvailableSubSlot);
		NewUnit->SetCurrentTile(BaseTile, AvailableSubSlot);

		UE_LOG(LogTemp, Log, TEXT("SpawnUnitAtBase: Spawned %s at (%d, %d) sub-slot %d"),
			*UnitTypeID.ToString(), BaseCoord.X, BaseCoord.Y, AvailableSubSlot);
	}

	return NewUnit;
}

ALairPlayerState* ALairGameMode::GetPlayerState(int32 PlayerIndex) const
{
	if (PlayerIndex >= 0 && PlayerIndex < PlayerStates.Num())
	{
		return PlayerStates[PlayerIndex];
	}
	return nullptr;
}

void ALairGameMode::EndCurrentTurn()
{
	if (TurnManager)
	{
		TurnManager->EndTurn();
	}
}
