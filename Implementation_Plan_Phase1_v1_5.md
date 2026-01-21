# LAIR - Implementation Plan Phase 1 (Enhanced)
**Foundation & Core Systems**

---

## ⚠️ SPECIFICATION LOCK NOTICE

**🔒 This document is LOCKED at v1.5**

Any change to this document requires a version bump to v1.6 or higher.

Do NOT make silent edits. If you need to modify this specification:
1. Create a new version (v1.6)
2. Document changes in Version History section
3. Update all references in other documents
4. Tag Git commit with version number

**Current Status:** LOCKED - Production specification for Phase 1 implementation

---

## Document Metadata

**Document Type:** Primary Implementation Authority for Phase 1  
**Version:** 1.5 (Quick Reference Added, Consolidated Phase1_At_A_Glance)  
**Last Updated:** January 21, 2026  
**Compatible Engine:** Unreal Engine 5.3.2  
**Status:** LOCKED - Ready for Implementation  
**Document Owner:** Development Team  

---

## Document Hierarchy

**THIS IS THE PRIMARY SOURCE OF TRUTH FOR PHASE 1 IMPLEMENTATION**

| Document | Purpose | Authority Level |
|----------|---------|-----------------|
| **Implementation_Plan_Phase1_v1.5.md** (this doc) | Phase 1 build instructions, file order, acceptance tests | **PRIMARY** |
| LAIR_UE5_Technical_Specification.md | Architecture reference, data structures, full system design | Reference |
| LAIR_Quick_Start_Guide.md | Getting started, onboarding, troubleshooting | Getting Started Only |
| LAIR_Rules_Glossary_v1.0.md | Game terminology, rule clarifications, Phase 1 status | Developer Reference |
| Master_Build_Prompt_v3.0.md | Claude Code orchestration instructions | Build Orchestration |

**In case of conflicts:** This document takes precedence for Phase 1 implementation details.

---

## 📋 Quick Reference (TL;DR)

**For developers who need to see the essentials at a glance.**

### Phase 1 Scope Summary

| ✅ INCLUDED (MVP) | ❌ EXCLUDED (Future Phases) |
|-------------------|----------------------------|
| 2-player hotseat | Mining system (Phase 2) |
| 10×10 test board | Combat system (Phase 2) |
| 3 unit types (Miner, Wagon, Footman) | Monsters (Phase 2) |
| Purchase → Place → Turn flow | Mystery tiles (Phase 2) |
| Data-driven stats | Outposts/gates (Phase 3) |
| Primitive 3D meshes | Elite units (Phase 3) |
| Text-based UI | Polished art/audio (Phase 4) |
| PIE testing only | AI opponents (Future) |
| | Online multiplayer (Future) |

### Critical Build Order (Must Follow)

**⚠️ DO NOT SKIP OR REORDER THESE STEPS**

1. **LairDataStructs.h** → All enums & structs (no dependencies)
2. **LairGameMode.h/.cpp** → Central authority, owns systems
3. **Tile.h/.cpp** → Board building blocks
4. **Unit.h/.cpp** → Game pieces, placed on tiles
5. **LairPlayerState.h/.cpp** → Per-player data (gold, faction)
6. **BoardSystemComponent.h/.cpp** → Creates grid of tiles
7. **TurnManagerComponent.h/.cpp** → Phase sequencing
8. **RulesEngineComponent.h/.cpp** → Validates actions
9. **LairPlayerController.h/.cpp** → Handles input
10. **Blueprint Setup** → BP_LairGameMode, BP_Tile, BP_Unit, TestMap

### 10 Acceptance Tests (Definition of Done)

Phase 1 is complete when all these tests pass:

| # | Test | Pass Criteria |
|---|------|---------------|
| 1 | Board Initialization | 10×10 grid spawns, bases at (0,0) and (9,9) |
| 2 | Data Table Loading | Footman stats load: Cost=50, Movement=5, HP=1 |
| 3 | Purchase (Affordable) | 200gp → buy Footman → 150gp, unit spawns |
| 4 | Purchase (Unaffordable) | 30gp → cannot buy 50gp Footman |
| 5 | Sub-Slot Capacity | 3 units + wagon (size 2) = rejected (need 5 slots, have 4) |
| 6 | Diagonal Movement Cost | Tile (5,5) → (6,6) = 2 movement points |
| 7 | Orthogonal Movement Cost | Tile (5,5) → (5,6) = 1 movement point |
| 8 | Turn Phase Progression | Purchase → Mining → Movement → Next Player |
| 9 | Placement Validation | Full tile (4 units) rejects new unit |
| 10 | Player Alternation | P1 → P2 → P1 → P2 cycles correctly |

### Stable API Contract (Frozen Functions)

**⚠️ DO NOT MODIFY these function signatures. Phase 2+ must extend, not break.**

**ATile (Tile.h):**
- `bool CanPlaceUnit(int32 SubSlotSize) const`
- `int32 GetAvailableSubSlots() const`
- `bool PlaceUnitInSubSlot(AUnit* Unit, int32 SubSlotIndex)`
- `bool RemoveUnitFromSubSlot(AUnit* Unit)`
- `TArray<AUnit*> GetAllUnitsOnTile() const`

**AUnit (Unit.h):**
- `void InitializeFromDataTable(FName UnitTypeID, const FUnitData& Data)`
- `void ResetMovement()`
- `void SetCurrentTile(ATile* NewTile, int32 SubSlotIndex)`
- `bool IsAtTile(ATile* Tile) const`

**UBoardSystemComponent:**
- `void InitializeBoard(const FString& LayoutTablePath)`
- `ATile* GetTileAt(FIntPoint Coord) const`
- `TArray<ATile*> GetNeighborTiles(ATile* Tile) const`
- `int32 GetMovementCost(FIntPoint From, FIntPoint To) const`

**UTurnManagerComponent:**
- `void StartFirstTurn()`
- `void AdvancePhase()`
- `void EndTurn()`
- `ETurnPhase GetCurrentPhase() const`
- `int32 GetCurrentPlayerIndex() const`

**URulesEngineComponent:**
- `bool CanAffordUnit(int32 PlayerGold, FName UnitTypeID) const`
- `bool CanSpawnUnitAt(ATile* Tile, int32 SubSlotSize) const`
- `bool CanPlaceUnitAt(ATile* Tile, AUnit* Unit) const`

**If you need to change behavior:** Create `FunctionName_V2()`, don't modify the original!

### Quick Data Reference

**DT_Units (3 rows for Phase 1):**
- **Miner**: 40gp, 4 movement, 1 HP, size 1
- **Wagon**: 70gp, 6 movement, 1 HP, size 2
- **Footman**: 50gp, 5 movement, 1 HP, size 1

**Movement Costs:**
- Orthogonal (N/S/E/W): 1 point
- Diagonal (NE/NW/SE/SW): 2 points

**Tile Capacity:**
- Each tile = 4 sub-slots
- Normal unit = 1 sub-slot
- Wagon = 2 sub-slots

---

## Phase 1 Objective

**Goal:** Establish a compilable, testable foundation with basic board, unit placement, and turn management.

**What IS in Phase 1 (MVP Scope):**
- ✅ 2-player hotseat only (no AI, no online)
- ✅ 10x10 test board with primitive visual assets
- ✅ Basic unit types: Miner, Wagon, Footman
- ✅ Purchase → Place → Turn advance flow
- ✅ Data-driven unit stats from Data Tables
- ✅ Primitive 3D meshes (cubes/cylinders acceptable)
- ✅ Basic UI (text-based acceptable)
- ✅ Playable in Unreal Editor (no standalone packaging required)

**What is NOT in Phase 1:**
- ❌ Mining system (Phase 2)
- ❌ Combat system (Phase 2)
- ❌ Monsters (Phase 2)
- ❌ Mystery tiles (Phase 2)
- ❌ Outposts/gates (Phase 3)
- ❌ Elite units & special abilities (Phase 3)
- ❌ Polished art/audio (Phase 4)
- ❌ AI opponents (Future)
- ❌ Online multiplayer (Future)
- ❌ Standalone .exe packaging (Phase 4)

**Success Criteria:**
- Compiles without errors
- Runs in PIE (Play In Editor) without crashes
- Can purchase 3 units and place them
- Can advance through turn phases
- All 10 acceptance tests pass (see Definition of Done section)

**Timeline:** 1-2 weeks for initial build, 1 week for iteration/testing

---

## System Architecture & Data Flow

### Data Flow Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                    PLAYER INPUT                              │
│                    (Mouse/Keyboard)                          │
└────────────────────┬────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────────┐
│              LairPlayerController                            │
│  - Detects tile clicks                                       │
│  - Detects unit clicks                                       │
│  - Converts input to game actions                            │
└────────────────────┬────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────────┐
│              LairGameMode (Orchestrator)                     │
│  - Owns all systems                                          │
│  - Validates actions via RulesEngine                         │
│  - Delegates execution to appropriate system                 │
└───────┬──────────┬──────────┬─────────────┬─────────────────┘
        │          │          │             │
        ▼          ▼          ▼             ▼
  ┌──────────┐ ┌────────┐ ┌──────────┐ ┌──────────┐
  │  Turn    │ │ Rules  │ │  Board   │ │  Data    │
  │ Manager  │ │ Engine │ │  System  │ │  Tables  │
  └─────┬────┘ └────┬───┘ └─────┬────┘ └──────────┘
        │           │           │
        │           │           ▼
        │           │      ┌─────────┐
        │           │      │  Tile   │◄────┐
        │           │      │ (Actor) │     │
        │           │      └─────────┘     │
        │           │           │          │
        │           │           │  Spawns  │
        │           │           ▼          │
        │           │      ┌─────────┐     │
        │           └─────►│  Unit   │─────┘
        │                  │ (Actor) │  References
        │                  └─────────┘
        │
        └─► Updates phase state
            Triggers phase change events
```

### Key Data Flow Paths

**Purchase Flow:**
```
Player clicks "Buy Footman"
    → PlayerController::OnPurchaseButtonClicked()
    → GameMode::PurchaseUnit(PlayerID, "Footman")
    → RulesEngine::CanAffordUnit(PlayerGold, "Footman")
    → [if yes] PlayerState::DeductGold(50)
    → GameMode::SpawnUnitAtBase(PlayerID, "Footman")
    → BoardSystem::GetTileAt(BaseCoord)
    → Tile::PlaceUnitInSubSlot(NewUnit, SubSlot)
    → Unit spawns, position updated
```

**Turn Advance Flow:**
```
Player clicks "End Turn"
    → PlayerController::OnEndTurnClicked()
    → GameMode::EndCurrentTurn()
    → TurnManager::AdvancePhase()
    → [if end of phases] TurnManager::AdvancePlayer()
    → TurnManager::SetPhase(Purchase)
    → Broadcasts OnPhaseChanged event
    → UI updates
```

**Movement Validation Flow (Phase 2+, but architecture exists):**
```
Player clicks unit, then clicks target tile
    → PlayerController::OnTileClicked(TargetTile)
    → GameMode::MoveUnit(SelectedUnit, TargetTile)
    → RulesEngine::CanUnitMoveToTile(Unit, TargetTile)
    → BoardSystem::GetMovementCost(FromCoord, ToCoord)
    → [if valid] Unit::MoveTo(TargetTile, SubSlot)
    → BoardSystem updates unit position
```

---

## Critical: File Generation Order

**Follow this order exactly.** Dependencies are carefully sequenced.

### Step 1: Data Structures & Enums (Foundation)
**File:** `Source/Lair/Public/LairDataStructs.h`

**Why First:** All other files depend on these type definitions.

**Contents:**
- All `UENUM` definitions (EUnitType, EArmyFaction, ETurnPhase, etc.)
- All `USTRUCT` definitions (FUnitData, FTileData, etc.)
- No implementation code, only declarations

**Validation:** Compiles standalone (no dependencies)

---

### Step 2: Game Mode (Authority)
**File:** `Source/Lair/Public/LairGameMode.h`  
**File:** `Source/Lair/Private/LairGameMode.cpp`

**Why Second:** Central authority that owns all systems.

**Responsibilities:**
- Initialize game state
- Own all component systems
- Hold references to Data Tables
- Start first turn

**Dependencies:** LairDataStructs.h

**Key Functions:**
```cpp
void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
void BeginPlay() override;
void StartGame();
```

**Validation:** Can create BP_LairGameMode blueprint that inherits from ALairGameMode

---

### Step 3: Tile Actor (Board Building Block)
**File:** `Source/Lair/Public/Tile.h`  
**File:** `Source/Lair/Private/Tile.cpp`

**Why Third:** Board system needs tiles to spawn.

**Responsibilities:**
- Represent one grid square
- Track 4 sub-slots (quarters)
- Track tile type (empty, mining, gate, etc.)
- Visual mesh representation

**Dependencies:** LairDataStructs.h

**Key Functions:**
```cpp
void Initialize(FIntPoint GridCoord, FName TileTypeID);
bool CanPlaceUnit(int32 SubSlotSize) const;
int32 GetAvailableSubSlots() const;
bool PlaceUnitInSubSlot(AUnit* Unit, int32 SubSlotIndex);
bool RemoveUnitFromSubSlot(AUnit* Unit);
TArray<AUnit*> GetAllUnitsOnTile() const;
```

**Phase 1 Implementation Notes:**
- Visual: Simple plane mesh with grid material
- Sub-slots visible as 4 colored quadrants (for debugging)
- Tile type changes color (empty=white, base=blue/red)

---

### Step 4: Unit Actor (Game Pieces)
**File:** `Source/Lair/Public/Unit.h`  
**File:** `Source/Lair/Private/Unit.cpp`

**Why Fourth:** Units need tiles to exist first.

**Responsibilities:**
- Represent game pieces (Miner, Footman, Wagon)
- Load stats from Data Table (cost, movement, HP, sub-slot size)
- Track current tile and sub-slot position
- Visual mesh representation

**Dependencies:** LairDataStructs.h, Tile.h (forward declared)

**Key Functions:**
```cpp
void InitializeFromDataTable(FName UnitTypeID, const FUnitData& Data);
void SetCurrentTile(ATile* NewTile, int32 SubSlotIndex);
bool IsAtTile(ATile* Tile) const;
void ResetMovement();
int32 GetRemainingMovement() const;
```

**Phase 1 Implementation Notes:**
- Visual: Simple cylinder mesh with faction color material
- Miners = short cylinders
- Footmen = medium cylinders
- Wagons = wide rectangular prisms (2 sub-slots wide)
- Display unit type text above mesh

---

### Step 5: Player State (Per-Player Data)
**File:** `Source/Lair/Public/LairPlayerState.h`  
**File:** `Source/Lair/Private/LairPlayerState.cpp`

**Why Fifth:** Units and systems need to query player resources.

**Responsibilities:**
- Track player gold
- Track player faction (Dwarves, Humans, etc.)
- Track units owned by player
- Expose gold modification functions

**Dependencies:** LairDataStructs.h

**Key Functions:**
```cpp
int32 GetGold() const;
void AddGold(int32 Amount);
void DeductGold(int32 Amount);
bool CanAfford(int32 Cost) const;
EArmyFaction GetFaction() const;
void SetFaction(EArmyFaction NewFaction);
```

**Phase 1 Implementation Notes:**
- Starting gold: 200gp
- Gold changes broadcast events for UI updates
- No gold cap in Phase 1

---

### Step 6: Board System Component (Grid Management)
**File:** `Source/Lair/Public/BoardSystemComponent.h`  
**File:** `Source/Lair/Private/BoardSystemComponent.cpp`

**Why Sixth:** Now we have tiles, units, and players to manage.

**Responsibilities:**
- Spawn grid of tiles from board layout data table
- Track tile positions in 2D grid (TMap<FIntPoint, ATile*>)
- Provide tile lookup by coordinate
- Calculate movement costs between tiles
- Identify neighboring tiles

**Dependencies:** LairDataStructs.h, Tile.h, Unit.h (forward declared)

**Key Functions:**
```cpp
void InitializeBoard(const FString& LayoutTablePath);
ATile* GetTileAt(FIntPoint Coord) const;
TArray<ATile*> GetNeighborTiles(ATile* Tile) const;
int32 GetMovementCost(FIntPoint From, FIntPoint To) const;
FIntPoint GetPlayerBaseCoord(int32 PlayerIndex) const;
```

**Phase 1 Implementation Notes:**
- Loads DT_BoardLayout_Test10x10 (10x10 grid)
- Player 1 base at (0, 0)
- Player 2 base at (9, 9)
- Orthogonal neighbors cost 1, diagonal neighbors cost 2
- Tiles spawn at Z=0, 100 units apart (grid scale)

**Data Table Format:**
```cpp
USTRUCT(BlueprintType)
struct FBoardLayoutRow : public FTableRowBase
{
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FIntPoint GridCoord;  // (X, Y) position
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TileTypeID;  // Row name from DT_TileTypes
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PlayerBaseIndex;  // -1 if not a base, 0 for P1, 1 for P2
};
```

---

### Step 7: Turn Manager Component (Phase Sequencing)
**File:** `Source/Lair/Public/TurnManagerComponent.h`  
**File:** `Source/Lair/Private/TurnManagerComponent.cpp`

**Why Seventh:** Board and players exist, now we need turn flow.

**Responsibilities:**
- Track current turn phase (Purchase, Mining, Movement)
- Track current player index
- Advance phases and players
- Broadcast phase/player change events
- Count total turns

**Dependencies:** LairDataStructs.h

**Key Functions:**
```cpp
void StartFirstTurn();
void AdvancePhase();
void EndTurn();
ETurnPhase GetCurrentPhase() const;
int32 GetCurrentPlayerIndex() const;
int32 GetTurnNumber() const;
```

**Phase 1 Implementation Notes:**
- Turn order: Purchase → Mining → Movement → Next Player
- 2 players (indices 0 and 1)
- Phase changes broadcast events for UI
- Turn counter starts at 1

**Events:**
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, ETurnPhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerChanged, int32, NewPlayerIndex);
```

---

### Step 8: Rules Engine Component (Validation Logic)
**File:** `Source/Lair/Public/RulesEngineComponent.h`  
**File:** `Source/Lair/Private/RulesEngineComponent.cpp`

**Why Eighth:** Systems exist, now enforce game rules.

**Responsibilities:**
- Validate unit purchases (gold check)
- Validate unit placement (sub-slot availability)
- Validate movement (Phase 2+, architecture exists)
- Validate actions based on current phase
- Load and cache unit data from Data Tables

**Dependencies:** LairDataStructs.h, Tile.h, Unit.h, BoardSystemComponent.h (forward declared)

**Key Functions:**
```cpp
void Initialize(UDataTable* UnitsTable, UDataTable* TileTypesTable);
bool CanAffordUnit(int32 PlayerGold, FName UnitTypeID) const;
bool CanSpawnUnitAt(ATile* Tile, int32 SubSlotSize) const;
bool CanPlaceUnitAt(ATile* Tile, AUnit* Unit) const;
FUnitData GetUnitData(FName UnitTypeID) const;
```

**Phase 1 Implementation Notes:**
- Loads DT_Units and caches in memory
- Purchase validation: gold >= unit cost
- Placement validation: available sub-slots >= unit size
- Phase validation exists but is simple (Purchase phase = can buy)

---

### Step 9: Player Controller (Input Handling)
**File:** `Source/Lair/Public/LairPlayerController.h`  
**File:** `Source/Lair/Private/LairPlayerController.cpp`

**Why Ninth:** All systems ready, now handle player input.

**Responsibilities:**
- Detect mouse clicks on tiles
- Detect UI button clicks
- Convert input to game actions
- Communicate with GameMode to execute actions
- Enable mouse cursor

**Dependencies:** LairDataStructs.h, Tile.h, Unit.h (forward declared)

**Key Functions:**
```cpp
void SetupInputComponent() override;
void OnLeftMouseClick();
void OnPurchaseButtonClicked(FName UnitTypeID);
void OnEndTurnClicked();
ATile* GetTileUnderCursor() const;
```

**Phase 1 Implementation Notes:**
- Mouse cursor always visible
- Left click selects tile
- UI buttons trigger purchases
- "End Turn" button advances turn
- Line trace on "TileTrace" collision channel

**Input Setup:**
```cpp
void ALairPlayerController::BeginPlay()
{
    Super::BeginPlay();
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    DefaultMouseCursor = EMouseCursor::Default;
}
```

---

### Step 10: Blueprint Setup & Testing
**Files to Create:**
- `Content/Blueprints/GameMode/BP_LairGameMode.uasset`
- `Content/Blueprints/Actors/BP_Tile.uasset`
- `Content/Blueprints/Actors/BP_Unit.uasset`
- `Content/Maps/TestMap_10x10.umap`
- `Content/DataTables/DT_Units.uasset`
- `Content/DataTables/DT_TileTypes.uasset`
- `Content/DataTables/DT_Factions.uasset`
- `Content/DataTables/DT_BoardLayout_Test10x10.uasset`

**Why Last:** C++ classes must compile first before Blueprints can inherit.

**Blueprint Setup Instructions:**

**BP_LairGameMode:**
1. Inherit from `ALairGameMode`
2. Assign Data Tables:
   - UnitsTable → DT_Units
   - TileTypesTable → DT_TileTypes
   - FactionsTable → DT_Factions
   - BoardLayoutTable → DT_BoardLayout_Test10x10
3. Set TileClass → BP_Tile
4. Set UnitClass → BP_Unit

**BP_Tile:**
1. Inherit from `ATile`
2. Add Static Mesh Component:
   - Mesh: Engine default plane (100x100)
   - Material: Simple colored material
   - Collision: Block TileTrace channel

**BP_Unit:**
1. Inherit from `AUnit`
2. Add Static Mesh Component:
   - Mesh: Engine default cylinder
   - Material: Colored by faction (set in code)
   - Collision: Block Visibility

**TestMap_10x10:**
1. Create new level
2. Set World Settings → GameMode Override → BP_LairGameMode
3. Add basic lighting (Directional Light)
4. Add camera (static, top-down view)
5. Set camera position: X=450, Y=450, Z=800
6. Set camera rotation: Pitch=-60

**Data Table Setup:**

See "Data Table Schemas" section below for exact row structures.

---

## Data Table Schemas

### DT_Units

**Row Structure:** `FUnitData` (inherits from `FTableRowBase`)

```cpp
USTRUCT(BlueprintType)
struct FUnitData : public FTableRowBase
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
    FText DisplayName;  // "Footman", "Miner", "Wagon"
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
    int32 Cost;  // Gold cost to purchase
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
    int32 MovementPoints;  // How far unit can move per turn
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
    int32 HitPoints;  // Damage unit can take (all 1 in Phase 1)
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
    int32 SubSlotSize;  // 1 for normal units, 2 for wagons
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
    EArmyFaction AvailableToFaction;  // Which faction can buy this (Any for Phase 1)
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit|Phase2")
    TArray<int32> AttackDiceValues;  // Combat dice (Phase 2+, leave empty for now)
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit|Phase2")
    int32 NumberOfDice;  // How many dice to roll (Phase 2+, set to 1)
};
```

**Phase 1 Rows:**

| Row Name | DisplayName | Cost | Movement | HP | SubSlotSize | Faction |
|----------|-------------|------|----------|----|----|---------|
| Miner | Miner | 40 | 4 | 1 | 1 | Any |
| Wagon | Wagon | 70 | 6 | 1 | 2 | Any |
| Footman | Footman | 50 | 5 | 1 | 1 | Any |

---

### DT_TileTypes

**Row Structure:** `FTileTypeData` (inherits from `FTableRowBase`)

```cpp
USTRUCT(BlueprintType)
struct FTileTypeData : public FTableRowBase
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
    FText DisplayName;  // "Empty", "Mining", "Player Base"
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
    bool bWalkable;  // Can units move onto this tile?
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
    FLinearColor DebugColor;  // Visual color in Phase 1
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Phase2")
    bool bCanMine;  // Can miners draw mining cards here? (Phase 2+)
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Phase2")
    bool bHasGate;  // Does this tile have a gate structure? (Phase 2+)
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Phase3")
    bool bIsOutpost;  // Can units spawn here when captured? (Phase 3+)
};
```

**Phase 1 Rows:**

| Row Name | DisplayName | Walkable | DebugColor | CanMine | HasGate |
|----------|-------------|----------|------------|---------|---------|
| Empty | Empty Tile | true | White (1,1,1,1) | false | false |
| PlayerBase | Player Base | true | Blue/Red | false | false |

**Note:** PlayerBase color is set dynamically based on player index:
- Player 0 (P1) = Blue (0, 0, 1, 1)
- Player 1 (P2) = Red (1, 0, 0, 1)

---

### DT_Factions

**Row Structure:** `FFactionData` (inherits from `FTableRowBase`)

```cpp
USTRUCT(BlueprintType)
struct FFactionData : public FTableRowBase
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
    FText DisplayName;  // "Dwarves", "Humans", etc.
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
    FLinearColor FactionColor;  // Unit color tint
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
    int32 StartingGold;  // 200 for Phase 1
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction|Phase3")
    TArray<FName> EliteUnitTypes;  // Faction-specific units (Phase 3+, leave empty)
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction|Phase3")
    FText FactionAbilityDescription;  // Special ability text (Phase 3+, leave blank)
};
```

**Phase 1 Rows:**

| Row Name | DisplayName | FactionColor | StartingGold | EliteUnits |
|----------|-------------|--------------|--------------|------------|
| Player1 | Player 1 | Blue (0,0.5,1,1) | 200 | [] |
| Player2 | Player 2 | Red (1,0.2,0,1) | 200 | [] |

**Note:** Phase 1 uses generic "Player 1" and "Player 2" instead of Dwarves/Humans. Faction names change in Phase 3.

---

### DT_BoardLayout_Test10x10

**Row Structure:** `FBoardLayoutRow` (inherits from `FTableRowBase`)

```cpp
USTRUCT(BlueprintType)
struct FBoardLayoutRow : public FTableRowBase
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
    FIntPoint GridCoord;  // (X, Y) grid position
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
    FName TileTypeID;  // Row name from DT_TileTypes
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
    int32 PlayerBaseIndex;  // -1 if not a base, 0 for P1, 1 for P2
};
```

**Phase 1 Layout (100 rows):**

Simplified pseudo-code for data table generation:
```
For X from 0 to 9:
    For Y from 0 to 9:
        Row Name: "Tile_X_Y"
        GridCoord: (X, Y)
        
        If (X == 0 AND Y == 0):
            TileTypeID: "PlayerBase"
            PlayerBaseIndex: 0
        Else If (X == 9 AND Y == 9):
            TileTypeID: "PlayerBase"
            PlayerBaseIndex: 1
        Else:
            TileTypeID: "Empty"
            PlayerBaseIndex: -1
```

**Example Rows:**

| Row Name | GridCoord | TileTypeID | PlayerBaseIndex |
|----------|-----------|------------|-----------------|
| Tile_0_0 | (0, 0) | PlayerBase | 0 |
| Tile_0_1 | (0, 1) | Empty | -1 |
| Tile_5_5 | (5, 5) | Empty | -1 |
| Tile_9_9 | (9, 9) | PlayerBase | 1 |

---

## Acceptance Tests (Definition of Done)

Phase 1 is complete when all 10 tests pass.

### Test 1: Board Initialization
**Given:** Launch TestMap_10x10 in PIE  
**When:** Game starts  
**Then:**
- 100 tiles spawn (10x10 grid)
- Player 1 base exists at coordinate (0, 0) - blue tile
- Player 2 base exists at coordinate (9, 9) - red tile
- All other tiles are white (Empty type)
- No compile errors, no runtime crashes

**Validation Method:**
```cpp
// In test or debug console
UBoardSystemComponent* Board = GameMode->GetBoardSystem();
ATile* P1Base = Board->GetTileAt(FIntPoint(0, 0));
check(P1Base != nullptr);
check(P1Base->GetTileTypeID() == FName("PlayerBase"));
```

---

### Test 2: Data Table Loading
**Given:** Game has started  
**When:** Query unit data for "Footman"  
**Then:**
- Cost = 50
- MovementPoints = 5
- HitPoints = 1
- SubSlotSize = 1
- Data loads without errors

**Validation Method:**
```cpp
URulesEngineComponent* Rules = GameMode->GetRulesEngine();
FUnitData FootmanData = Rules->GetUnitData(FName("Footman"));
check(FootmanData.Cost == 50);
check(FootmanData.MovementPoints == 5);
```

---

### Test 3: Purchase Unit (Affordable)
**Given:**
- Player 1 is active
- Player has 200 gold
- Purchase phase is active

**When:** Player purchases Footman (costs 50 gold)  
**Then:**
- Gold becomes 150
- Footman spawns at Player 1 base (0, 0)
- Footman occupies 1 sub-slot
- 3 sub-slots remain free

**Validation Method:**
```cpp
ALairPlayerState* P1State = GameMode->GetPlayerState(0);
int32 GoldBefore = P1State->GetGold();  // 200
GameMode->PurchaseUnit(0, FName("Footman"));
int32 GoldAfter = P1State->GetGold();  // 150
check(GoldAfter == GoldBefore - 50);

ATile* P1Base = Board->GetTileAt(FIntPoint(0, 0));
check(P1Base->GetAllUnitsOnTile().Num() == 1);
```

---

### Test 4: Purchase Unit (Unaffordable)
**Given:**
- Player 1 is active
- Player has 30 gold
- Purchase phase is active

**When:** Player attempts to purchase Footman (costs 50 gold)  
**Then:**
- Purchase is rejected
- Gold remains 30
- No unit spawns
- Error message displayed to player

**Validation Method:**
```cpp
P1State->SetGold(30);
bool bSuccess = GameMode->PurchaseUnit(0, FName("Footman"));
check(bSuccess == false);
check(P1State->GetGold() == 30);  // Unchanged
```

---

### Test 5: Sub-Slot Capacity Validation
**Given:**
- Tile has 3 footmen (3 sub-slots occupied)
- 1 sub-slot remains free

**When:** Player attempts to place Wagon (requires 2 sub-slots)  
**Then:**
- Placement is rejected
- Error message: "Not enough space on tile"
- Wagon does not spawn

**Validation Method:**
```cpp
ATile* TestTile = Board->GetTileAt(FIntPoint(5, 5));
// Place 3 footmen manually
for (int i = 0; i < 3; ++i) {
    AUnit* Footman = SpawnUnit(FName("Footman"));
    TestTile->PlaceUnitInSubSlot(Footman, i);
}

// Try to place wagon (size 2)
AUnit* Wagon = SpawnUnit(FName("Wagon"));
bool bCanPlace = TestTile->CanPlaceUnit(Wagon->GetSubSlotSize());
check(bCanPlace == false);
```

---

### Test 6: Diagonal Movement Cost
**Given:**
- Board is initialized
- Tiles at (5, 5) and (6, 6) exist

**When:** Calculate movement cost from (5, 5) to (6, 6)  
**Then:**
- Movement cost = 2 (diagonal movement)

**Validation Method:**
```cpp
UBoardSystemComponent* Board = GameMode->GetBoardSystem();
int32 Cost = Board->GetMovementCost(FIntPoint(5, 5), FIntPoint(6, 6));
check(Cost == 2);
```

---

### Test 7: Orthogonal Movement Cost
**Given:**
- Board is initialized
- Tiles at (5, 5) and (5, 6) exist

**When:** Calculate movement cost from (5, 5) to (5, 6)  
**Then:**
- Movement cost = 1 (orthogonal movement)

**Validation Method:**
```cpp
int32 Cost = Board->GetMovementCost(FIntPoint(5, 5), FIntPoint(5, 6));
check(Cost == 1);
```

---

### Test 8: Turn Phase Progression
**Given:** Game has started  
**When:** Player clicks "End Turn" 3 times  
**Then:**
- Phase 1: Purchase
- Phase 2: Mining (click)
- Phase 3: Movement (click)
- Phase 1: Purchase, Player 2's turn (click)

**Validation Method:**
```cpp
UTurnManagerComponent* TurnMgr = GameMode->GetTurnManager();
check(TurnMgr->GetCurrentPhase() == ETurnPhase::Purchase);
TurnMgr->AdvancePhase();
check(TurnMgr->GetCurrentPhase() == ETurnPhase::Mining);
TurnMgr->AdvancePhase();
check(TurnMgr->GetCurrentPhase() == ETurnPhase::Movement);
TurnMgr->AdvancePhase();
check(TurnMgr->GetCurrentPhase() == ETurnPhase::Purchase);
check(TurnMgr->GetCurrentPlayerIndex() == 1);  // Player 2
```

---

### Test 9: Tile Capacity Full
**Given:**
- Tile has 4 footmen (4 sub-slots occupied)
- Tile is completely full

**When:** Player attempts to place another footman  
**Then:**
- Placement is rejected
- Tile still has 4 units
- No new unit spawns

**Validation Method:**
```cpp
ATile* TestTile = Board->GetTileAt(FIntPoint(5, 5));
for (int i = 0; i < 4; ++i) {
    AUnit* Footman = SpawnUnit(FName("Footman"));
    TestTile->PlaceUnitInSubSlot(Footman, i);
}
check(TestTile->GetAvailableSubSlots() == 0);

AUnit* ExtraFootman = SpawnUnit(FName("Footman"));
bool bCanPlace = TestTile->CanPlaceUnit(1);
check(bCanPlace == false);
```

---

### Test 10: Player Alternation
**Given:** Game has started  
**When:** Complete 4 full turns  
**Then:**
- Turn 1: Player 1 (index 0)
- Turn 2: Player 2 (index 1)
- Turn 3: Player 1 (index 0)
- Turn 4: Player 2 (index 1)

**Validation Method:**
```cpp
UTurnManagerComponent* TurnMgr = GameMode->GetTurnManager();
check(TurnMgr->GetCurrentPlayerIndex() == 0);
TurnMgr->EndTurn();  // P1 done
check(TurnMgr->GetCurrentPlayerIndex() == 1);
TurnMgr->EndTurn();  // P2 done
check(TurnMgr->GetCurrentPlayerIndex() == 0);
TurnMgr->EndTurn();  // P1 done
check(TurnMgr->GetCurrentPlayerIndex() == 1);
```

---

## Phase 1 → Phase 2 Transition Contract

### What Phase 1 Delivers (Battle-Ready Foundation)

1. **Board System is Complete**
   - 10x10 grid spawns correctly
   - Tile placement works
   - Grid coordinate lookup works
   - Movement cost calculation exists (but movement not fully implemented)
   - Extendable to 20x20 board by changing data table

2. **Unit System is Proven**
   - Units spawn from data tables
   - Sub-slot placement logic is robust
   - Unit tracking per tile works
   - Visual representation is functional

3. **Turn Management is Solid**
   - Phase progression works
   - Player rotation works
   - Turn counting works
   - Phase change events fire correctly

4. **Data Table System is Proven**
   - Units load from DT_Units
   - Tile types load from DT_TileTypes
   - Factions load from DT_Factions
   - Board layouts load from DT_BoardLayout_*
   - Adding new data tables follows same pattern

5. **Unit/Tile Actor Framework is Stable**
   - Units can be spawned, placed, and tracked
   - Tiles can query their contents
   - Position updates work
   - Visual representation is functional

### What Phase 2 Must Add (Without Breaking Phase 1)

**Phase 2 Implementation Strategy:**

1. **Mining System**
   - ADD: `UMiningSystemComponent` (new component)
   - ADD: `URulesEngine::CanMineAt()` (new function)
   - ADD: `ATile::MiningState` (new property)
   - **DO NOT MODIFY:** Core tile placement/query functions

2. **Combat System**
   - ADD: `UCombatResolverComponent` (new component)
   - ADD: `URulesEngine::CanAttackTile()` (new function)
   - ADD: `AUnit::TakeDamage()` (new function)
   - **DO NOT MODIFY:** Core unit initialization/placement

3. **Monsters**
   - ADD: `AMonster` class (inherits from base Actor)
   - ADD: `URulesEngine::CheckForCombatTrigger()` (new function)
   - **DO NOT MODIFY:** Unit movement core logic

### Phase 2 Must NOT Do

❌ Change ATile::CanPlaceUnit() signature  
❌ Change AUnit::SetCurrentTile() signature  
❌ Change UBoardSystemComponent::GetTileAt() signature  
❌ Change ETurnPhase enum values (Purchase, Mining, Movement)  
❌ Change PlayerState gold tracking implementation  
❌ Remove or rename any Phase 1 public API functions  

### Migration Path for Breaking Changes

If Phase 2 MUST modify a Phase 1 API:

1. Create new versioned function: `CanPlaceUnit_V2()`
2. Mark old function with `DEPRECATED()` macro
3. Update all Phase 1 callsites to use V2
4. Document in migration notes
5. Remove deprecated function in Phase 3

**Example:**
```cpp
// Phase 1 - Original
DEPRECATED(1.2, "Use CanPlaceUnit_V2 for combat-aware placement")
UFUNCTION(BlueprintCallable, Category = "Tile")
bool CanPlaceUnit(int32 SubSlotSize) const;

// Phase 2 - New version
UFUNCTION(BlueprintCallable, Category = "Tile")
bool CanPlaceUnit_V2(int32 SubSlotSize, bool bCheckForEnemies = false) const;
```

### Phase 2 Data Dependencies

**Phase 2 will add these data tables:**
- DT_Monsters (new)
- DT_MiningCards (new)
- DT_Minerals (new)
- DT_Treasures (new)
- DT_MysteryTiles (new)

**Phase 2 will expand existing tables:**
- DT_Units (add AttackDiceValues, NumberOfDice, HitPoints)
- DT_TileTypes (add bCanMine, bHasGate)

**Phase 1 tables remain unchanged in structure**, only new rows may be added.

### Acceptance Tests for Phase 2

Phase 2 is complete when:
- All Phase 1 acceptance tests STILL PASS (regression check)
- 10 new Phase 2 acceptance tests pass (mining, combat, monsters)
- No breaking changes to Phase 1 public APIs

---

## Common Pitfalls & Solutions

### Pitfall 1: Circular Dependencies

**Problem:** Header A includes Header B, Header B includes Header A → Compile error

**Solution:** Use forward declarations
```cpp
// In .h file
class ATile;  // Forward declaration

// In .cpp file
#include "Tile.h"  // Full include
```

### Pitfall 2: Blueprint Not Finding C++ Class

**Problem:** Create BP_LairGameMode but can't find ALairGameMode parent class

**Solution:**
1. Compile C++ code first
2. Restart editor
3. Check that class has `UCLASS()` macro
4. Verify module dependencies in `.Build.cs`

### Pitfall 3: Data Table Row Not Found

**Problem:** `GetUnitData("Footman")` returns null even though row exists

**Solution:**
1. Check row name matches exactly (case-sensitive)
2. Verify data table is assigned in BP_LairGameMode
3. Check that struct name matches (FUnitData vs UnitData)
4. Use `DataTable->GetAllRows()` to debug available rows

### Pitfall 4: Tiles Not Spawning

**Problem:** `InitializeBoard()` runs but no tiles appear

**Solution:**
1. Check Output Log for spawn errors
2. Verify `TileClass` is assigned in BoardSystemComponent
3. Check spawn collision settings (use `SpawnActorDeferred` if needed)
4. Verify World pointer is valid

### Pitfall 5: Mouse Input Not Working

**Problem:** Can't click on tiles

**Solution:**
1. Enable mouse cursor in Player Controller:
   ```cpp
   bShowMouseCursor = true;
   bEnableClickEvents = true;
   ```
2. Add collision to tile meshes
3. Set collision channel to "WorldStatic" or "WorldDynamic"
4. Verify input mode is set to "Game and UI"

---

## Glossary

**For detailed game terminology and rule clarifications, see:** `LAIR_Rules_Glossary_v1.0.md`

| Term | Definition |
|------|------------|
| **Sub-Slot** | One quarter of a tile; tiles have 4 sub-slots for unit placement |
| **Tile** | One grid square on the board; base unit of board system |
| **TileType** | Category of tile (Empty, Mining, Gate, PlayerBase, etc.) |
| **Unit** | Game piece representing a soldier, miner, or wagon |
| **Wagon** | Special unit that occupies 2 sub-slots; used for mineral transport |
| **Elite Unit** | Faction-specific unit with special abilities (Phase 3+) |
| **Mining Tile** | Tile where miners can draw mining cards (Phase 2+) |
| **Mystery Tile** | Hidden tile revealed when entered (Phase 2+) |
| **Outpost** | Capturable location for spawning units (Phase 3+) |
| **Gate** | Defensive structure requiring breach roll to pass (Phase 2+) |
| **Player Base** | Starting position where units spawn |
| **Orthogonal** | North, South, East, West directions (cost 1 movement) |
| **Diagonal** | NE, NW, SE, SW directions (cost 2 movement) |
| **PIE** | Play In Editor; testing mode in Unreal Editor |
| **Data Table** | Unreal asset for storing structured data (CSV-like) |
| **Blueprint (BP)** | Visual scripting asset in Unreal |
| **Component** | Reusable logic/data container attached to actors |
| **Stable API** | Public function whose signature will not change between phases |

---

## Project Settings Baseline (Phase 1)

### DefaultGame.ini Settings

```ini
[/Script/EngineSettings.GameMapsSettings]
GameDefaultMap=/Game/Maps/TestMap_10x10.TestMap_10x10
EditorStartupMap=/Game/Maps/TestMap_10x10.TestMap_10x10
GlobalDefaultGameMode=/Game/Blueprints/GameMode/BP_LairGameMode.BP_LairGameMode_C

[/Script/Engine.Engine]
bUseFixedFrameRate=False
FixedFrameRate=60.000000

[/Script/Engine.CollisionProfile]
+DefaultChannelResponses=(Channel=ECC_GameTraceChannel1,Name="TileTrace",DefaultResponse=ECR_Block,bTraceType=True)
```

### Input Configuration

**Mouse:**
- Left Click → Select/Confirm
- Right Click → Cancel (future)
- Scroll Wheel → Camera zoom

**Keyboard:**
- WASD → Camera pan
- Q/E → Camera rotate (future)
- Space → End turn (optional hotkey)

### Collision Presets

- **Tiles:** WorldStatic, block TileTrace channel
- **Units:** WorldDynamic, block Visibility
- **Camera:** No collision

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | Jan 20, 2026 | Initial Phase 1 plan |
| 1.1 | Jan 21, 2026 | Post-audit enhancements: Rules clarifications, board layout, acceptance tests, glossary |
| 1.2 | Jan 21, 2026 | Final audit: Data flow diagram, stable API contract, Phase 1→2 transition contract, acceptance test count fixed (10 tests) |
| 1.3 | Jan 21, 2026 | Spec Lock banner added, production-ready |
| 1.4 | Jan 21, 2026 | Quick Reference section added, integrated Phase1_At_A_Glance content, reference to Rules Glossary added |
| 1.5 | Jan 21, 2026 | Version governance fix: Properly bumped version after content additions, unified all cross-doc references to v1.5 |

---

## Next Steps After Phase 1

Once all 10 acceptance tests pass:

1. **Commit to GitHub:** Tag as `v0.1-phase1-complete`
2. **Lock these docs as v1.5:** Treat further changes as versioned updates
3. **Playtest:** Have 2 people play 3 games hotseat
4. **Document Issues:** Create GitHub issues for bugs found
5. **Plan Phase 2:** Mining system, combat, monsters (following transition contract)
6. **Update Documentation:** Create Implementation_Plan_Phase2_v1.0.md

**Do NOT start Phase 2 until Phase 1 is stable and tested!**

---

**End of Phase 1 Implementation Plan v1.5**

**Status: LOCKED - Ready for Implementation**
