// Tile.cpp
// Step 3: Tile Actor (Board Building Block)

#include "Tile.h"
#include "Unit.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

ATile::ATile()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create root component
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootSceneComponent);

	// Create tile mesh
	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	TileMesh->SetupAttachment(RootSceneComponent);

	// Use default plane mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane"));
	if (PlaneMesh.Succeeded())
	{
		TileMesh->SetStaticMesh(PlaneMesh.Object);
	}

	// Scale plane to tile size (default plane is 100x100, we want TILE_WORLD_SIZE)
	float Scale = LairConstants::TILE_WORLD_SIZE / 100.0f;
	TileMesh->SetRelativeScale3D(FVector(Scale, Scale, 1.0f));

	// Enable collision for tile trace
	TileMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TileMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	TileMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// Initialize sub-slots array (4 slots, all null = empty)
	SubSlots.Init(nullptr, LairConstants::TILE_SUB_SLOTS);

	// Default values
	GridCoord = FIntPoint::ZeroValue;
	TileTypeID = FName("Empty");
	PlayerBaseIndex = -1;
}

void ATile::BeginPlay()
{
	Super::BeginPlay();

	// Create dynamic material for color changes
	if (TileMesh && TileMesh->GetMaterial(0))
	{
		DynamicMaterial = TileMesh->CreateAndSetMaterialInstanceDynamic(0);
	}

	UpdateVisuals();
}

void ATile::Initialize(FIntPoint InGridCoord, FName InTileTypeID)
{
	GridCoord = InGridCoord;
	TileTypeID = InTileTypeID;

	UE_LOG(LogTemp, Verbose, TEXT("ATile::Initialize - Tile at (%d, %d) type: %s"),
		GridCoord.X, GridCoord.Y, *TileTypeID.ToString());
}

bool ATile::CanPlaceUnit(int32 SubSlotSize) const
{
	if (SubSlotSize <= 0 || SubSlotSize > LairConstants::TILE_SUB_SLOTS)
	{
		return false;
	}

	int32 AvailableSlots = GetAvailableSubSlots();

	// For size 1 units, just need any empty slot
	if (SubSlotSize == 1)
	{
		return AvailableSlots >= 1;
	}

	// For wagons (size 2), need 2 contiguous empty slots
	if (SubSlotSize == 2)
	{
		// Check for contiguous pairs: (0,1), (1,2), (2,3)
		for (int32 i = 0; i < LairConstants::TILE_SUB_SLOTS - 1; ++i)
		{
			if (SubSlots[i] == nullptr && SubSlots[i + 1] == nullptr)
			{
				return true;
			}
		}
		return false;
	}

	// For larger sizes (future-proofing)
	return AvailableSlots >= SubSlotSize;
}

int32 ATile::GetAvailableSubSlots() const
{
	int32 Count = 0;
	for (int32 i = 0; i < SubSlots.Num(); ++i)
	{
		if (SubSlots[i] == nullptr)
		{
			Count++;
		}
	}
	return Count;
}

bool ATile::PlaceUnitInSubSlot(AUnit* Unit, int32 SubSlotIndex)
{
	if (!Unit)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATile::PlaceUnitInSubSlot - Unit is null"));
		return false;
	}

	if (SubSlotIndex < 0 || SubSlotIndex >= SubSlots.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("ATile::PlaceUnitInSubSlot - Invalid SubSlotIndex %d"), SubSlotIndex);
		return false;
	}

	if (SubSlots[SubSlotIndex] != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATile::PlaceUnitInSubSlot - SubSlot %d is already occupied"), SubSlotIndex);
		return false;
	}

	// Get unit size to handle wagons (size 2) properly
	int32 UnitSize = Unit->GetSubSlotSize();

	// For wagons (size 2), verify and occupy both slots
	if (UnitSize == 2)
	{
		if (SubSlotIndex >= SubSlots.Num() - 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("ATile::PlaceUnitInSubSlot - Cannot place size 2 unit at slot %d"), SubSlotIndex);
			return false;
		}
		if (SubSlots[SubSlotIndex + 1] != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ATile::PlaceUnitInSubSlot - SubSlot %d is already occupied (wagon needs 2 slots)"), SubSlotIndex + 1);
			return false;
		}
		// Occupy both slots for wagon
		SubSlots[SubSlotIndex] = Unit;
		SubSlots[SubSlotIndex + 1] = Unit;
	}
	else
	{
		SubSlots[SubSlotIndex] = Unit;
	}

	// Position unit at sub-slot location (for wagons, center between the two slots)
	FVector UnitPosition = GetActorLocation() + GetSubSlotOffset(SubSlotIndex);
	if (UnitSize == 2)
	{
		// Center wagon between the two slots
		FVector Slot2Offset = GetSubSlotOffset(SubSlotIndex + 1);
		UnitPosition = GetActorLocation() + (GetSubSlotOffset(SubSlotIndex) + Slot2Offset) * 0.5f;
	}
	UnitPosition.Z += 50.0f; // Raise unit above tile
	Unit->SetActorLocation(UnitPosition);

	UE_LOG(LogTemp, Verbose, TEXT("ATile::PlaceUnitInSubSlot - Placed unit (size %d) in slot %d at tile (%d, %d)"),
		UnitSize, SubSlotIndex, GridCoord.X, GridCoord.Y);

	return true;
}

bool ATile::RemoveUnitFromSubSlot(AUnit* Unit)
{
	if (!Unit)
	{
		return false;
	}

	bool bFound = false;
	// Remove from ALL slots (handles wagons that occupy 2 slots)
	for (int32 i = 0; i < SubSlots.Num(); ++i)
	{
		if (SubSlots[i] == Unit)
		{
			SubSlots[i] = nullptr;
			UE_LOG(LogTemp, Verbose, TEXT("ATile::RemoveUnitFromSubSlot - Removed unit from slot %d at tile (%d, %d)"),
				i, GridCoord.X, GridCoord.Y);
			bFound = true;
		}
	}

	if (!bFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATile::RemoveUnitFromSubSlot - Unit not found on this tile"));
	}
	return bFound;
}

TArray<AUnit*> ATile::GetAllUnitsOnTile() const
{
	TArray<AUnit*> Units;
	for (int32 i = 0; i < SubSlots.Num(); ++i)
	{
		if (SubSlots[i] != nullptr && !Units.Contains(SubSlots[i]))
		{
			// Avoid duplicates for wagons that occupy 2 slots
			Units.Add(SubSlots[i]);
		}
	}
	return Units;
}

void ATile::SetTileColor(FLinearColor Color)
{
	if (!DynamicMaterial && TileMesh)
	{
		// Create dynamic material if not already created
		DynamicMaterial = TileMesh->CreateAndSetMaterialInstanceDynamic(0);
	}

	if (DynamicMaterial)
	{
		// Try common material parameter names used in UE5
		// Default lit materials often use "BaseColor" or "Base Color"
		DynamicMaterial->SetVectorParameterValue(FName("BaseColor"), Color);
		DynamicMaterial->SetVectorParameterValue(FName("Base Color"), Color);
		// Also set emissive for visibility in case base color doesn't work
		DynamicMaterial->SetVectorParameterValue(FName("EmissiveColor"), Color * 0.3f);
	}
}

void ATile::SetHighlight(bool bHighlighted, FLinearColor Color)
{
	if (bHighlighted)
	{
		SetTileColor(Color);
	}
	else
	{
		// Reset to default color using centralized logic
		UpdateVisuals();
	}
}

void ATile::UpdateVisuals()
{
	// Priority: PlayerBase color > TileType DebugColor > White default
	if (PlayerBaseIndex == 0)
	{
		SetTileColor(FLinearColor(0.0f, 0.0f, 1.0f, 1.0f)); // Blue for P1
	}
	else if (PlayerBaseIndex == 1)
	{
		SetTileColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)); // Red for P2
	}
	else if (CachedTileTypeData.DebugColor != FLinearColor::White)
	{
		// Use tile type debug color if set (non-white)
		SetTileColor(CachedTileTypeData.DebugColor);
	}
	else
	{
		SetTileColor(FLinearColor::White); // Default white for empty
	}
}

void ATile::SetTileTypeData(const FTileTypeData& InTileTypeData)
{
	CachedTileTypeData = InTileTypeData;
	UpdateVisuals();
}

int32 ATile::FindAvailableSubSlot(int32 SubSlotSize) const
{
	if (SubSlotSize == 2)
	{
		// For wagons (size 2), need contiguous slots
		for (int32 i = 0; i < LairConstants::TILE_SUB_SLOTS - 1; ++i)
		{
			if (SubSlots[i] == nullptr && SubSlots[i + 1] == nullptr)
			{
				return i;
			}
		}
	}
	else
	{
		// For size 1 units, find any empty slot
		for (int32 i = 0; i < SubSlots.Num(); ++i)
		{
			if (SubSlots[i] == nullptr)
			{
				return i;
			}
		}
	}
	return -1;
}

FVector ATile::GetSubSlotOffset(int32 SubSlotIndex) const
{
	// Sub-slots are arranged as quadrants:
	// 0 = NW (-X, +Y)
	// 1 = NE (+X, +Y)
	// 2 = SW (-X, -Y)
	// 3 = SE (+X, -Y)
	float HalfSize = LairConstants::TILE_WORLD_SIZE * 0.25f;

	switch (SubSlotIndex)
	{
	case 0: return FVector(-HalfSize, HalfSize, 0.0f);  // NW
	case 1: return FVector(HalfSize, HalfSize, 0.0f);   // NE
	case 2: return FVector(-HalfSize, -HalfSize, 0.0f); // SW
	case 3: return FVector(HalfSize, -HalfSize, 0.0f);  // SE
	default: return FVector::ZeroVector;
	}
}
