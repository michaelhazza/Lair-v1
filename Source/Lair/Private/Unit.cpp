// Unit.cpp
// Step 4: Unit Actor (Game Pieces)

#include "Unit.h"
#include "Tile.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create root component
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootSceneComponent);

	// Create unit mesh
	UnitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnitMesh"));
	UnitMesh->SetupAttachment(RootSceneComponent);

	// Use default cylinder mesh for units
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		UnitMesh->SetStaticMesh(CylinderMesh.Object);
	}

	// Default scale for normal unit
	UnitMesh->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.3f));

	// Enable collision for visibility
	UnitMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	UnitMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	UnitMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// Default values
	UnitTypeID = NAME_None;
	OwnerPlayerIndex = 0;
	CurrentTile = nullptr;
	SubSlotIndex = 0;
	RemainingMovement = 0;
	CurrentHP = 1;
}

void AUnit::BeginPlay()
{
	Super::BeginPlay();

	// Create dynamic material for color changes
	if (UnitMesh && UnitMesh->GetMaterial(0))
	{
		DynamicMaterial = UnitMesh->CreateAndSetMaterialInstanceDynamic(0);
	}

	UpdateVisuals();
}

void AUnit::InitializeFromDataTable(FName InUnitTypeID, const FUnitData& Data)
{
	UnitTypeID = InUnitTypeID;
	CachedUnitData = Data;

	// Initialize stats
	CurrentHP = Data.HitPoints;
	RemainingMovement = Data.MovementPoints;

	// Adjust scale based on unit type
	if (Data.SubSlotSize == 2)
	{
		// Wagon is wider
		UnitMesh->SetRelativeScale3D(FVector(0.35f, 0.25f, 0.25f));
	}
	else
	{
		// Normal unit
		UnitMesh->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.3f));
	}

	UpdateVisuals();

	UE_LOG(LogTemp, Log, TEXT("AUnit::InitializeFromDataTable - Initialized %s (Cost: %d, Movement: %d, HP: %d, Size: %d)"),
		*InUnitTypeID.ToString(), Data.Cost, Data.MovementPoints, Data.HitPoints, Data.SubSlotSize);
}

void AUnit::ResetMovement()
{
	RemainingMovement = CachedUnitData.MovementPoints;
	UE_LOG(LogTemp, Verbose, TEXT("AUnit::ResetMovement - Reset movement to %d"), RemainingMovement);
}

void AUnit::SetCurrentTile(ATile* NewTile, int32 InSubSlotIndex)
{
	CurrentTile = NewTile;
	SubSlotIndex = InSubSlotIndex;

	if (CurrentTile)
	{
		UE_LOG(LogTemp, Verbose, TEXT("AUnit::SetCurrentTile - Unit now at tile (%d, %d) sub-slot %d"),
			CurrentTile->GridCoord.X, CurrentTile->GridCoord.Y, SubSlotIndex);
	}
}

bool AUnit::IsAtTile(ATile* Tile) const
{
	return CurrentTile == Tile;
}

void AUnit::SetSelected(bool bSelected)
{
	if (bSelected)
	{
		// Highlight with brighter color
		SetUnitColor(FLinearColor(1.0f, 1.0f, 0.0f, 1.0f)); // Yellow highlight
	}
	else
	{
		// Restore normal color
		UpdateVisuals();
	}
}

void AUnit::SetUnitColor(FLinearColor Color)
{
	if (!DynamicMaterial && UnitMesh)
	{
		DynamicMaterial = UnitMesh->CreateAndSetMaterialInstanceDynamic(0);
	}

	if (DynamicMaterial)
	{
		// Try common material parameter names used in UE5
		DynamicMaterial->SetVectorParameterValue(FName("BaseColor"), Color);
		DynamicMaterial->SetVectorParameterValue(FName("Base Color"), Color);
		// Also set emissive for visibility in case base color doesn't work
		DynamicMaterial->SetVectorParameterValue(FName("EmissiveColor"), Color * 0.5f);
	}
}

void AUnit::UpdateVisuals()
{
	// Set color based on owner
	if (OwnerPlayerIndex == 0)
	{
		SetUnitColor(FLinearColor(0.2f, 0.4f, 1.0f, 1.0f)); // Blue for P1
	}
	else if (OwnerPlayerIndex == 1)
	{
		SetUnitColor(FLinearColor(1.0f, 0.3f, 0.2f, 1.0f)); // Red for P2
	}
	else
	{
		SetUnitColor(FLinearColor::Gray); // Neutral
	}
}
