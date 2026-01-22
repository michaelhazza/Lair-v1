// RulesEngineComponent.cpp
// Step 8: Rules Engine Component (Validation Logic)

#include "RulesEngineComponent.h"
#include "Tile.h"
#include "Unit.h"
#include "Engine/DataTable.h"

URulesEngineComponent::URulesEngineComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	UnitsDataTable = nullptr;
	TileTypesDataTable = nullptr;
}

void URulesEngineComponent::Initialize(UDataTable* InUnitsTable, UDataTable* InTileTypesTable)
{
	UnitsDataTable = InUnitsTable;
	TileTypesDataTable = InTileTypesTable;

	CacheDataFromTables();

	UE_LOG(LogTemp, Log, TEXT("URulesEngineComponent::Initialize - Cached %d unit types, %d tile types"),
		CachedUnitData.Num(), CachedTileTypeData.Num());
}

void URulesEngineComponent::CacheDataFromTables()
{
	CachedUnitData.Empty();
	CachedTileTypeData.Empty();

	// Cache unit data
	if (UnitsDataTable)
	{
		// Iterate by row name and use FindRow to ensure correct key/value pairing
		// (GetAllRows and GetRowNames may not return in the same order)
		TArray<FName> RowNames = UnitsDataTable->GetRowNames();
		for (const FName& RowName : RowNames)
		{
			FUnitData* UnitRow = UnitsDataTable->FindRow<FUnitData>(RowName, TEXT("CacheDataFromTables"));
			if (UnitRow)
			{
				CachedUnitData.Add(RowName, *UnitRow);
				UE_LOG(LogTemp, Verbose, TEXT("Cached unit: %s (Cost: %d, Movement: %d)"),
					*RowName.ToString(), UnitRow->Cost, UnitRow->MovementPoints);
			}
		}
	}
	else
	{
		// Create default unit data for Phase 1 testing
		FUnitData MinerData;
		MinerData.DisplayName = FText::FromString(TEXT("Miner"));
		MinerData.Cost = 40;
		MinerData.MovementPoints = 4;
		MinerData.HitPoints = 1;
		MinerData.SubSlotSize = 1;
		CachedUnitData.Add(FName("Miner"), MinerData);

		FUnitData WagonData;
		WagonData.DisplayName = FText::FromString(TEXT("Wagon"));
		WagonData.Cost = 70;
		WagonData.MovementPoints = 6;
		WagonData.HitPoints = 1;
		WagonData.SubSlotSize = 2;
		CachedUnitData.Add(FName("Wagon"), WagonData);

		FUnitData FootmanData;
		FootmanData.DisplayName = FText::FromString(TEXT("Footman"));
		FootmanData.Cost = 50;
		FootmanData.MovementPoints = 5;
		FootmanData.HitPoints = 1;
		FootmanData.SubSlotSize = 1;
		CachedUnitData.Add(FName("Footman"), FootmanData);

		UE_LOG(LogTemp, Log, TEXT("URulesEngineComponent::CacheDataFromTables - Created default unit data"));
	}

	// Cache tile type data
	if (TileTypesDataTable)
	{
		// Iterate by row name and use FindRow to ensure correct key/value pairing
		// (GetAllRows and GetRowNames may not return in the same order)
		TArray<FName> RowNames = TileTypesDataTable->GetRowNames();
		for (const FName& RowName : RowNames)
		{
			FTileTypeData* TileRow = TileTypesDataTable->FindRow<FTileTypeData>(RowName, TEXT("CacheDataFromTables"));
			if (TileRow)
			{
				CachedTileTypeData.Add(RowName, *TileRow);
			}
		}
	}
	else
	{
		// Create default tile type data
		FTileTypeData EmptyTile;
		EmptyTile.DisplayName = FText::FromString(TEXT("Empty"));
		EmptyTile.bWalkable = true;
		EmptyTile.DebugColor = FLinearColor::White;
		CachedTileTypeData.Add(FName("Empty"), EmptyTile);

		FTileTypeData PlayerBaseTile;
		PlayerBaseTile.DisplayName = FText::FromString(TEXT("Player Base"));
		PlayerBaseTile.bWalkable = true;
		PlayerBaseTile.DebugColor = FLinearColor::Blue;
		CachedTileTypeData.Add(FName("PlayerBase"), PlayerBaseTile);

		UE_LOG(LogTemp, Log, TEXT("URulesEngineComponent::CacheDataFromTables - Created default tile type data"));
	}
}

bool URulesEngineComponent::CanAffordUnit(int32 PlayerGold, FName UnitTypeID) const
{
	const FUnitData* UnitData = CachedUnitData.Find(UnitTypeID);
	if (!UnitData)
	{
		UE_LOG(LogTemp, Warning, TEXT("URulesEngineComponent::CanAffordUnit - Unknown unit type: %s"),
			*UnitTypeID.ToString());
		return false;
	}

	bool bCanAfford = PlayerGold >= UnitData->Cost;

	UE_LOG(LogTemp, Verbose, TEXT("URulesEngineComponent::CanAffordUnit - %s costs %d, player has %d: %s"),
		*UnitTypeID.ToString(), UnitData->Cost, PlayerGold, bCanAfford ? TEXT("YES") : TEXT("NO"));

	return bCanAfford;
}

bool URulesEngineComponent::CanSpawnUnitAt(ATile* Tile, int32 SubSlotSize) const
{
	if (!Tile)
	{
		UE_LOG(LogTemp, Warning, TEXT("URulesEngineComponent::CanSpawnUnitAt - Tile is null"));
		return false;
	}

	bool bCanPlace = Tile->CanPlaceUnit(SubSlotSize);

	UE_LOG(LogTemp, Verbose, TEXT("URulesEngineComponent::CanSpawnUnitAt - Tile (%d, %d) for size %d: %s"),
		Tile->GridCoord.X, Tile->GridCoord.Y, SubSlotSize, bCanPlace ? TEXT("YES") : TEXT("NO"));

	return bCanPlace;
}

bool URulesEngineComponent::CanPlaceUnitAt(ATile* Tile, AUnit* Unit) const
{
	if (!Tile || !Unit)
	{
		return false;
	}

	return CanSpawnUnitAt(Tile, Unit->GetSubSlotSize());
}

FUnitData URulesEngineComponent::GetUnitData(FName UnitTypeID) const
{
	const FUnitData* UnitData = CachedUnitData.Find(UnitTypeID);
	if (UnitData)
	{
		return *UnitData;
	}

	UE_LOG(LogTemp, Warning, TEXT("URulesEngineComponent::GetUnitData - Unknown unit type: %s"),
		*UnitTypeID.ToString());

	return FUnitData();
}

FTileTypeData URulesEngineComponent::GetTileTypeData(FName TileTypeID) const
{
	const FTileTypeData* TileData = CachedTileTypeData.Find(TileTypeID);
	if (TileData)
	{
		return *TileData;
	}

	return FTileTypeData();
}

bool URulesEngineComponent::IsTileWalkable(ATile* Tile) const
{
	if (!Tile)
	{
		return false;
	}

	FTileTypeData TileData = GetTileTypeData(Tile->GetTileTypeID());
	return TileData.bWalkable;
}

TArray<FName> URulesEngineComponent::GetAllUnitTypes() const
{
	TArray<FName> UnitTypes;
	CachedUnitData.GetKeys(UnitTypes);
	return UnitTypes;
}
