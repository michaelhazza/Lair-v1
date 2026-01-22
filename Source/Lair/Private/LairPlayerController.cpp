// LairPlayerController.cpp
// Step 9: Player Controller (Input Handling)

#include "LairPlayerController.h"
#include "LairGameMode.h"
#include "TurnManagerComponent.h"
#include "Tile.h"
#include "Unit.h"
#include "Kismet/GameplayStatics.h"

ALairPlayerController::ALairPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	DefaultMouseCursor = EMouseCursor::Default;

	SelectedTile = nullptr;
	SelectedUnit = nullptr;
	GameModeRef = nullptr;
}

void ALairPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Cache game mode reference
	GameModeRef = Cast<ALairGameMode>(UGameplayStatics::GetGameMode(this));

	// Ensure mouse cursor is visible
	bShowMouseCursor = true;
	bEnableClickEvents = true;

	// Set input mode to game and UI
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	UE_LOG(LogTemp, Log, TEXT("ALairPlayerController::BeginPlay - Controller initialized"));
}

void ALairPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		// Bind left mouse click
		InputComponent->BindAction("LeftMouseClick", IE_Pressed, this, &ALairPlayerController::OnLeftMouseClick);

		// Bind right mouse click
		InputComponent->BindAction("RightMouseClick", IE_Pressed, this, &ALairPlayerController::OnRightMouseClick);
	}
}

void ALairPlayerController::OnLeftMouseClick()
{
	UE_LOG(LogTemp, Verbose, TEXT("ALairPlayerController::OnLeftMouseClick"));

	// First check for unit under cursor
	AUnit* UnitUnderCursor = GetUnitUnderCursor();
	if (UnitUnderCursor)
	{
		SelectUnit(UnitUnderCursor);
		return;
	}

	// Then check for tile under cursor
	ATile* TileUnderCursor = GetTileUnderCursor();
	if (TileUnderCursor)
	{
		SelectTile(TileUnderCursor);
	}
}

void ALairPlayerController::OnRightMouseClick()
{
	UE_LOG(LogTemp, Verbose, TEXT("ALairPlayerController::OnRightMouseClick"));

	// Right click clears selection
	ClearSelection();
}

void ALairPlayerController::OnPurchaseButtonClicked(FName UnitTypeID)
{
	if (!GameModeRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("ALairPlayerController::OnPurchaseButtonClicked - GameMode is null"));
		return;
	}

	int32 PlayerIndex = GetLocalPlayerIndex();

	bool bSuccess = GameModeRef->PurchaseUnit(PlayerIndex, UnitTypeID);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("ALairPlayerController::OnPurchaseButtonClicked - Player %d purchased %s"),
			PlayerIndex, *UnitTypeID.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ALairPlayerController::OnPurchaseButtonClicked - Purchase failed for %s"),
			*UnitTypeID.ToString());
	}
}

void ALairPlayerController::OnEndTurnClicked()
{
	if (!GameModeRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("ALairPlayerController::OnEndTurnClicked - GameMode is null"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("ALairPlayerController::OnEndTurnClicked - Ending turn"));

	GameModeRef->EndCurrentTurn();
	ClearSelection();
}

void ALairPlayerController::OnAdvancePhaseClicked()
{
	if (!GameModeRef)
	{
		return;
	}

	UTurnManagerComponent* TurnManager = GameModeRef->GetTurnManager();
	if (TurnManager)
	{
		TurnManager->AdvancePhase();
		UE_LOG(LogTemp, Log, TEXT("ALairPlayerController::OnAdvancePhaseClicked - Phase advanced"));
	}
}

ATile* ALairPlayerController::GetTileUnderCursor() const
{
	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	if (HitResult.bBlockingHit)
	{
		ATile* Tile = Cast<ATile>(HitResult.GetActor());
		return Tile;
	}

	return nullptr;
}

AUnit* ALairPlayerController::GetUnitUnderCursor() const
{
	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	if (HitResult.bBlockingHit)
	{
		AUnit* Unit = Cast<AUnit>(HitResult.GetActor());
		return Unit;
	}

	return nullptr;
}

int32 ALairPlayerController::GetLocalPlayerIndex() const
{
	// In hotseat mode, return the current player from TurnManager
	if (GameModeRef)
	{
		UTurnManagerComponent* TurnManager = GameModeRef->GetTurnManager();
		if (TurnManager)
		{
			return TurnManager->GetCurrentPlayerIndex();
		}
	}

	return 0;
}

void ALairPlayerController::SelectTile(ATile* Tile)
{
	if (SelectedTile == Tile)
	{
		return;
	}

	// Clear previous selection highlight
	if (SelectedTile)
	{
		SelectedTile->SetHighlight(false);
	}

	SelectedTile = Tile;

	if (SelectedTile)
	{
		SelectedTile->SetHighlight(true, FLinearColor::Green);
		UE_LOG(LogTemp, Verbose, TEXT("ALairPlayerController::SelectTile - Selected tile at (%d, %d)"),
			SelectedTile->GridCoord.X, SelectedTile->GridCoord.Y);
	}

	OnTileSelected.Broadcast(SelectedTile);
}

void ALairPlayerController::SelectUnit(AUnit* Unit)
{
	if (SelectedUnit == Unit)
	{
		return;
	}

	// Clear previous selection highlight
	if (SelectedUnit)
	{
		SelectedUnit->SetSelected(false);
	}

	SelectedUnit = Unit;

	if (SelectedUnit)
	{
		SelectedUnit->SetSelected(true);
		UE_LOG(LogTemp, Verbose, TEXT("ALairPlayerController::SelectUnit - Selected unit %s"),
			*SelectedUnit->UnitTypeID.ToString());

		// Also select the tile the unit is on
		if (SelectedUnit->CurrentTile)
		{
			SelectTile(SelectedUnit->CurrentTile);
		}
	}

	OnUnitSelected.Broadcast(SelectedUnit);
}

void ALairPlayerController::ClearSelection()
{
	if (SelectedTile)
	{
		SelectedTile->SetHighlight(false);
		SelectedTile = nullptr;
	}

	if (SelectedUnit)
	{
		SelectedUnit->SetSelected(false);
		SelectedUnit = nullptr;
	}

	OnTileSelected.Broadcast(nullptr);
	OnUnitSelected.Broadcast(nullptr);

	UE_LOG(LogTemp, Verbose, TEXT("ALairPlayerController::ClearSelection - Selection cleared"));
}
