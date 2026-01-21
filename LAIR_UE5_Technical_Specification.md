# LAIR - Unreal Engine 5 Technical Specification
**Version 1.1 - Architecture Reference**

---

## Document Metadata

**Document Type:** Architecture Reference & System Design  
**Version:** 1.1 (Post-Audit Update)  
**Last Updated:** January 21, 2026  
**Compatible Engine:** Unreal Engine 5.3.2  
**Status:** Final - Reference Document  
**Document Owner:** Development Team  

---

## Document Hierarchy

**THIS IS THE ARCHITECTURE REFERENCE DOCUMENT**

| Document | Purpose | Authority Level |
|----------|---------|-----------------|
| Implementation_Plan_Phase1_v1.5.md | Phase 1 build instructions, file order, acceptance tests | **PRIMARY for Phase 1** |
| **LAIR_UE5_Technical_Specification.md (this doc)** | Architecture reference, data structures, full system design | **REFERENCE** |
| LAIR_Quick_Start_Guide.md | Getting started, onboarding, troubleshooting | Getting Started Only |
| LAIR_Rules_Glossary_v1.0.md | Game terminology, rule clarifications | Developer Reference |
| Master_Build_Prompt_v3.0.md | Claude Code orchestration instructions | Build Orchestration |

**Purpose of This Document:**
- Define overall system architecture (all phases)
- Document all data table structures
- Define C++ class interfaces
- Describe Blueprint integration points
- Provide long-term technical vision

**For Implementation:** Follow Implementation_Plan_Phase1_v1.5.md for exact file generation order and Phase 1 scope.

---

## Table of Contents
1. [System Architecture](#system-architecture)
2. [Data-Driven Systems](#data-driven-systems)
3. [C++ Class Architecture](#cpp-class-architecture)
4. [Blueprint Systems](#blueprint-systems)
5. [Game State Management](#game-state-management)
6. [Implementation Phases](#implementation-phases)
7. [File Structure](#file-structure)
8. [Testing Strategy](#testing-strategy)

---

## System Architecture

### High-Level Architecture
```
┌─────────────────────────────────────────────────────────┐
│                    Game Mode (C++)                       │
│  - Turn management, rule enforcement, state authority    │
└──────────────┬──────────────────────────────────────────┘
               │
    ┌──────────┴───────────┬──────────────┬───────────────┐
    │                      │              │               │
┌───▼────────┐  ┌─────────▼─────┐  ┌────▼─────┐  ┌──────▼──────┐
│   Board    │  │  Turn Manager │  │  Rules   │  │   Data      │
│   System   │  │    (C++)      │  │  Engine  │  │   Tables    │
│   (C++)    │  └───────────────┘  │  (C++)   │  │  (Assets)   │
└────────────┘                      └──────────┘  └─────────────┘
     │
     ├─ Grid Management
     ├─ Tile System
     ├─ Unit Placement
     └─ Pathfinding
```

### Core Systems Overview

| System | Language | Purpose | Dependencies |
|--------|----------|---------|--------------|
| **GameMode** | C++ | Authority, turn order, victory | All systems |
| **BoardSystem** | C++ | Grid management, tile logic | Data tables |
| **TurnManager** | C++ | Phase sequencing, player rotation | GameMode |
| **RulesEngine** | C++ | Movement validation, combat resolution | Data tables |
| **UnitManager** | C++ | Unit spawning, tracking, destruction | BoardSystem |
| **MiningSystem** | C++ | Card drawing, outcome resolution | Data tables |
| **CombatResolver** | C++ | Dice rolling, damage calculation | Data tables |
| **UI System** | Blueprint | HUD, menus, player interactions | All C++ systems |
| **Camera Controller** | Blueprint | Top-down view, zoom, rotation | None |

---

## Data-Driven Systems

### Data Table Definitions

All gameplay values must be externalized into Unreal Data Tables. This allows designers to balance the game without code changes.

#### 1. DT_Units (Data Table)
**Structure: FUnitData**

```cpp
USTRUCT(BlueprintType)
struct FUnitData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName UnitID;  // e.g., "Footman", "Miner", "Assassin"

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Cost;  // Gold cost to purchase

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MovementPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 HitPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int32> AttackDiceValues;  // e.g., [4,5,6] means hits on 4, 5, or 6

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 NumberOfDice;  // How many dice this unit rolls

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EUnitType UnitType;  // Enum: Miner, Wagon, Footman, Elite

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EArmyFaction Faction;  // Enum: Dwarves, Humans, Gnomes, Orcs, DarkElves, Goblins, Neutral

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SubSlotSize;  // 1 for units, 2 for wagons

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> SpecialAbilities;  // e.g., ["Infiltrate", "Defence"]

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMesh* UnitMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInterface* UnitMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* UnitIcon;
};
```

**Sample Data:**
| UnitID | DisplayName | Cost | MovementPoints | HP | AttackDiceValues | NumberOfDice | SubSlotSize | SpecialAbilities |
|--------|-------------|------|----------------|----|--------------------|--------------|-------------|------------------|
| Footman | Footman | 50 | 5 | 1 | [4,5,6] | 1 | 1 | [] |
| Miner | Miner | 40 | 4 | 1 | [5,6] | 1 | 1 | [] |
| Wagon | Mining Cart | 70 | 6 | 1 | [6] | 1 | 2 | [] |
| Assassin | Assassin | 100 | 7 | 1 | [4,5,6] | 2 | 1 | [Infiltrate] |
| Knight | Knight | 90 | 6 | 1 | [3,4,5,6] | 1 | 1 | [Defence] |
| Saboteur | Saboteur | 90 | 6 | 1 | [2,3,4,5,6] | 1 | 1 | [Kamikaze] |
| Archer | Archers | 80 | 5 | 1 | [4,5,6] | 1 | 1 | [RangedAttack] |
| SteamCart | Steam Cart | 110 | 7 | 1 | [5,6] | 1 | 2 | [SteamPower] |
| MasterMiner | Master Miner | 80 | 5 | 1 | [5,6] | 2 | 1 | [MasterMining] |

#### 2. DT_Monsters (Data Table)
**Structure: FMonsterData**

```cpp
USTRUCT(BlueprintType)
struct FMonsterData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName MonsterID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText FlavorText;  // e.g., "A giant charges you!"

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 HitPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int32> AttackDiceValues;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 NumberOfDice;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bRegeneratesHP;  // True for most monsters

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMesh* MonsterMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* MonsterIcon;
};
```

**Sample Data:**
| MonsterID | DisplayName | FlavorText | HP | AttackDiceValues | NumberOfDice | bRegeneratesHP |
|-----------|-------------|------------|----|--------------------|--------------|----------------|
| Dragon | Wykercles | Your heart grips in despair! | 10 | [2,3,4,5,6] | 1 | true |
| Giant | Giant | A giant charges you! | 3 | [4,5,6] | 1 | true |
| Banshee | Banshee | Her shrill cry calls out. | 1 | [4,5,6] | 1 | true |
| GiantRats | Giant Rats | Rats crawl out from amongst the rocks... | 1 | [6] | 2 | true |
| GiantSpider | Giant Spider | A clicking noise is heard...?!? | 1 | [5,6] | 2 | true |
| Vampire | Vampire | A shadow swirls above... | 2 | [3,4,5,6] | 1 | true |
| Mummy | Mummy | A foul stench comes from ahead... | 2 | [4,5,6] | 2 | true |
| Lizardman | Lizardman | You have stumbled across an angry Lizardman | 1 | [5,6] | 1 | true |
| Hellhound | Hellhound | A Hellhound Attack! | 1 | [4,5,6] | 1 | true |
| Zombies | Zombies | A murmuring gets louder... | 3 | [6] | 3 | true |
| Skeletons | Skeletons | Skeletons emerge and attack | 2 | [5,6] | 2 | true |
| Wraith | Wraith | You sense an evil presence... | 2 | [4,5,6] | 1 | true |
| Golem | Golem | The earth moves at your feet. | 3 | [4,5,6] | 1 | true |

#### 3. DT_MiningCards (Data Table)
**Structure: FMiningCardData**

```cpp
USTRUCT(BlueprintType)
struct FMiningCardData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName CardID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EMiningOutcome OutcomeType;  // Enum: Mineral, Treasure, Monster, MinedOut, DigDeeper

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName OutcomeID;  // References other tables (mineral name, monster ID, treasure ID)

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 GoldValue;  // For minerals

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* CardArt;
};
```

**Sample Data Distribution:**
| OutcomeType | Count | Examples |
|-------------|-------|----------|
| Mineral | 88 | IronOre (22), Copper (20), Silver (17), Gold (15), Platinum (9), Gems (5) |
| Treasure | 8 | TeardropNecklace, MurclaricsTomb, GnoremsChariot, etc. |
| Monster | 10 | Giant, Banshee, Vampire, Mummy, etc. |
| MinedOut | 6 | N/A |
| DigDeeper | 8 | N/A |

#### 4. DT_Minerals (Data Table)
**Structure: FMineralData**

```cpp
USTRUCT(BlueprintType)
struct FMineralData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName MineralID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 GoldValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* MineralIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor MineralColor;
};
```

**Sample Data:**
| MineralID | DisplayName | GoldValue |
|-----------|-------------|-----------|
| IronOre | Iron Ore | 50 |
| Copper | Copper | 70 |
| Silver | Silver | 90 |
| Gold | Gold | 120 |
| Platinum | Platinum | 200 |
| Gems | Jewels/Gems | 350 |

#### 5. DT_Treasures (Data Table)
**Structure: FTreasureData**

```cpp
USTRUCT(BlueprintType)
struct FTreasureData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TreasureID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ETreasureEffect EffectType;  // Enum: ImmediateGold, SpawnUnits, AddWagon, OneTimeUse, MultiUse

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 GoldAmount;  // For immediate gold treasures

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName UnitToSpawn;  // References DT_Units

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 UnitCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 UsesRemaining;  // -1 for unlimited, 0 for immediate, >0 for limited uses

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* TreasureIcon;
};
```

**Sample Data:**
| TreasureID | DisplayName | Description | EffectType | GoldAmount | UnitToSpawn | UnitCount | UsesRemaining |
|------------|-------------|-------------|------------|------------|-------------|-----------|---------------|
| TeardropNecklace | The Teardrop Necklace | This rare necklace was lost long ago | ImmediateGold | 500 | - | - | 0 |
| MurclaricsTomb | Murclaric's Tomb | The human priest befriended by Thorgrim | SpawnUnits | 0 | Footman | 4 | 0 |
| GnoremsChariot | Gnorem's Chariot | A magical chariot | AddWagon | 0 | Wagon | 1 | 0 |
| WandTransport | Wand of Magical Transport | Teleport a unit anywhere on board | OneTimeUse | 0 | - | - | 1 |
| StaffEnslavement | Staff of Enslavement | Convert monster to footman | MultiUse | 0 | - | - | 3 |
| OrbFervour | Orb of Fervour | All units +1 movement | MultiUse | 0 | - | - | 2 |
| KourtneyJewels | Kourtney Royal Jewels | Lost jewels of the King | ImmediateGold | 400 | - | - | 0 |
| ShadowRing | The Shadow Ring | Summon 3 shadow assassins | OneTimeUse | 0 | - | - | 1 |

#### 6. DT_MysteryTiles (Data Table)
**Structure: FMysteryTileData**

```cpp
USTRUCT(BlueprintType)
struct FMysteryTileData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TileID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EMysteryTileType TileType;  // Enum: Trap, Treasure, Monster, LostUnit

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText EventName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText EventDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName OutcomeReference;  // References other tables

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bBlocksTile;  // For cave-ins

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 DiceRollRequired;  // For saves (pit trap, bear trap)

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int32> SuccessValues;  // e.g., [4,5,6]

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* TileIcon;
};
```

**Sample Data:**
| TileID | TileType | EventName | EventDescription | OutcomeReference | bBlocksTile |
|--------|----------|-----------|------------------|------------------|-------------|
| CaveIn1 | Trap | Cave In | The roof collapses | - | true |
| CaveIn2 | Trap | Cave In | The roof collapses | - | true |
| PitTrap | Trap | Pit Trap | A false floor! A pit is revealed | - | false |
| BearTrap | Trap | Bear Traps | Iron claws reach out for your legs! | - | false |
| IllusionaryWind | Trap | Illusionary Wind | A strong gust of wind blows | - | false |
| TreasureGold | Treasure | Discarded Gold | 4 Discarded bags of Gold | 150gp | false |
| TreasureGems | Treasure | Sack of Gems | A Sack of Gems! | 200gp | false |
| MagicDrums | Treasure | Magic Drums | You have found the magic drums of Thorbald | MagicDrums | false |
| MagicHorn | Treasure | Magical Horn | The famous horn of Gidelius is yours! | MagicHorn | false |
| MonsterHellhound | Monster | Hellhounds | 2 Hellhounds Attack! | Hellhound | false |
| MonsterLizardman | Monster | Lizardman | You have stumbled across an angry Lizardman | Lizardman | false |
| MonsterGiant | Monster | Giant | A giant charges you! | Giant | false |
| MonsterBanshee | Monster | Banshee | Her shrill cry calls out. | Banshee | false |
| LostMiner | LostUnit | Lost Miner | A lost miner joins your forces | Miner | false |
| GateThief | LostUnit | The Gate Thief | You find the famed Gate Thief | GoldTheft | false |
| LostHero | LostUnit | Hero | You convince the hero to swear allegiance | EliteUnit | false |

#### 7. DT_Factions (Data Table)
**Structure: FFactionData**

```cpp
USTRUCT(BlueprintType)
struct FFactionData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName FactionID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Lore;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor FactionColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName EliteUnitType;  // References DT_Units

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> AvailableUnits;  // All units this faction can purchase

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* FactionBanner;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInterface* BaseMaterial;
};
```

**Sample Data:**
| FactionID | DisplayName | Color | EliteUnitType | AvailableUnits |
|-----------|-------------|-------|---------------|----------------|
| Dwarves | Dwarves of Silverlund | Brown | MasterMiner | [Miner, Wagon, Footman, MasterMiner] |
| Humans | Humans of Kourtnay | Blue | Knight | [Miner, Wagon, Footman, Knight] |
| Gnomes | Gnomes | Grey | SteamCart | [Miner, Wagon, Footman, SteamCart] |
| Orcs | Ukhruch Orcs | Red | Archer | [Miner, Wagon, Footman, Archer] |
| DarkElves | Dark Elves | Black | Assassin | [Miner, Wagon, Footman, Assassin] |
| Goblins | Goblin Tribes | Green | Saboteur | [Miner, Wagon, Footman, Saboteur] |

#### 8. DT_TileTypes (Data Table)
**Structure: FTileTypeData**

```cpp
USTRUCT(BlueprintType)
struct FTileTypeData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TileTypeID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsWalkable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCanMine;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHasGate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMesh* TileMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInterface* TileMaterial;
};
```

**Sample Data:**
| TileTypeID | DisplayName | bIsWalkable | bCanMine | bHasGate |
|------------|-------------|-------------|----------|----------|
| Empty | Empty | true | false | false |
| MiningTile | Mining Tile | true | true | false |
| MinedTile | Mined Tile | true | false | false |
| Gate | Gate | true | false | true |
| PlayerBase | Player Base | true | false | false |
| Lair | Lair (Dragon) | false | false | false |
| Temple | Temple of Baladin | true | false | false |
| Mystery | Mystery Tile | true | false | false |
| Outpost | Outpost | true | false | false |

#### 9. DT_VictoryConditions (Data Table)
**Structure: FVictoryConditionData**

```cpp
USTRUCT(BlueprintType)
struct FVictoryConditionData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ConditionID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EVictoryType VictoryType;  // Enum: GoldThreshold, Elimination, DragonTreasure

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 GoldRequired;  // For GoldThreshold

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bEnabled;  // Can be toggled per game mode
};
```

**Sample Data:**
| ConditionID | DisplayName | Description | VictoryType | GoldRequired | bEnabled |
|-------------|-------------|-------------|-------------|--------------|----------|
| GoldVictory | Amass 3000 Gold | Accumulate 3000 gold pieces | GoldThreshold | 3000 | true |
| EliminationVictory | Defeat All Armies | Eliminate all other players | Elimination | 0 | true |
| DragonVictory | Dragon's Hoard | Retrieve dragon treasure and return to base | DragonTreasure | 0 | true |

---

## C++ Class Architecture

### Core Hierarchy

```
ALairGameMode (Game Mode)
├── UBoardSystemComponent
├── UTurnManagerComponent
├── URulesEngineComponent
├── UMiningSystemComponent
├── UCombatResolverComponent
└── UVictoryManagerComponent

ALairPlayerController (Player Controller)
├── UInputComponent (handles mouse/keyboard)
└── ALairCameraActor (spawned camera)

ALairPlayerState (Player State)
├── Gold tracking
├── Unit ownership
├── Victory tracking
└── Faction data

ATile (Actor)
├── Grid coordinates
├── Tile type
├── Sub-slot occupancy (4 slots)
├── Mining state
└── Visual mesh/material

AUnit (Actor)
├── Unit data reference (FUnitData)
├── Current tile reference
├── Movement points remaining
├── HP tracking
└── Visual mesh/material

AMonster (Actor, inherits combat interface)
├── Monster data reference (FMonsterData)
├── Current tile reference
├── HP tracking
└── Visual mesh/material
```

### C++ Class Definitions

#### 1. ALairGameMode
**File: `LairGameMode.h` / `LairGameMode.cpp`**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LairGameMode.generated.h"

// Forward declarations
class UBoardSystemComponent;
class UTurnManagerComponent;
class URulesEngineComponent;
class UMiningSystemComponent;
class UCombatResolverComponent;
class UVictoryManagerComponent;

UCLASS()
class LAIR_API ALairGameMode : public AGameMode
{
    GENERATED_BODY()

public:
    ALairGameMode();

    virtual void BeginPlay() override;
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

    // Component references
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Systems")
    UBoardSystemComponent* BoardSystem;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Systems")
    UTurnManagerComponent* TurnManager;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Systems")
    URulesEngineComponent* RulesEngine;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Systems")
    UMiningSystemComponent* MiningSystem;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Systems")
    UCombatResolverComponent* CombatResolver;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Systems")
    UVictoryManagerComponent* VictoryManager;

    // Game configuration
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Config")
    int32 NumberOfPlayers;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Config")
    FIntPoint BoardSize;  // e.g., (20, 20) for a 20x20 grid

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Config")
    int32 StartingGold;

    // Data tables
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
    UDataTable* UnitsDataTable;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
    UDataTable* MonstersDataTable;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
    UDataTable* MiningCardsDataTable;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
    UDataTable* MineralsDataTable;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
    UDataTable* TreasuresDataTable;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
    UDataTable* MysteryTilesDataTable;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
    UDataTable* FactionsDataTable;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
    UDataTable* TileTypesDataTable;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
    UDataTable* VictoryConditionsDataTable;

    // Public API
    UFUNCTION(BlueprintCallable, Category = "Game")
    void StartGame();

    UFUNCTION(BlueprintCallable, Category = "Game")
    void CheckVictoryConditions();

    UFUNCTION(BlueprintPure, Category = "Game")
    ALairPlayerState* GetCurrentPlayer() const;
};
```

#### 2. UBoardSystemComponent
**File: `BoardSystemComponent.h` / `BoardSystemComponent.cpp`**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BoardSystemComponent.generated.h"

class ATile;
class AUnit;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAIR_API UBoardSystemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBoardSystemComponent();

    virtual void BeginPlay() override;

    // Board initialization
    UFUNCTION(BlueprintCallable, Category = "Board")
    void InitializeBoard(FIntPoint GridSize);

    // Tile access
    UFUNCTION(BlueprintPure, Category = "Board")
    ATile* GetTileAt(FIntPoint GridCoord) const;

    UFUNCTION(BlueprintPure, Category = "Board")
    TArray<ATile*> GetNeighborTiles(FIntPoint GridCoord, bool bIncludeDiagonal = true) const;

    UFUNCTION(BlueprintPure, Category = "Board")
    bool IsValidGridCoord(FIntPoint GridCoord) const;

    // Tile queries
    UFUNCTION(BlueprintPure, Category = "Board")
    bool IsTileOccupied(FIntPoint GridCoord) const;

    UFUNCTION(BlueprintPure, Category = "Board")
    int32 GetAvailableSubSlots(FIntPoint GridCoord) const;

    UFUNCTION(BlueprintCallable, Category = "Board")
    bool CanPlaceUnitAt(FIntPoint GridCoord, int32 SubSlotSize) const;

    // Unit placement
    UFUNCTION(BlueprintCallable, Category = "Board")
    bool PlaceUnitAt(AUnit* Unit, FIntPoint GridCoord, int32 SubSlotIndex);

    UFUNCTION(BlueprintCallable, Category = "Board")
    bool RemoveUnitFrom(AUnit* Unit, FIntPoint GridCoord);

    // Pathfinding
    UFUNCTION(BlueprintCallable, Category = "Board")
    TArray<FIntPoint> FindPath(FIntPoint Start, FIntPoint End, int32 MaxDistance) const;

    UFUNCTION(BlueprintPure, Category = "Board")
    int32 GetMovementCost(FIntPoint From, FIntPoint To) const;  // 1 for orthogonal, 2 for diagonal

private:
    // Grid storage (2D array of tiles)
    UPROPERTY()
    TMap<FIntPoint, ATile*> TileGrid;

    UPROPERTY()
    FIntPoint GridSize;

    // Tile spawning classes
    UPROPERTY(EditDefaultsOnly, Category = "Board")
    TSubclassOf<ATile> TileClass;

    UPROPERTY(EditDefaultsOnly, Category = "Board")
    float TileWorldSize;  // World space size of one tile (e.g., 100 units)

    // Helper functions
    bool IsOrthogonalMove(FIntPoint From, FIntPoint To) const;
    bool IsDiagonalMove(FIntPoint From, FIntPoint To) const;
};
```

#### 3. UTurnManagerComponent
**File: `TurnManagerComponent.h` / `TurnManagerComponent.cpp`**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TurnManagerComponent.generated.h"

UENUM(BlueprintType)
enum class ETurnPhase : uint8
{
    Purchase UMETA(DisplayName = "Purchase Phase"),
    Mining UMETA(DisplayName = "Mining Phase"),
    MovementCombat UMETA(DisplayName = "Movement & Combat Phase"),
    EndTurn UMETA(DisplayName = "End Turn")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, ETurnPhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerChanged, int32, NewPlayerIndex);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAIR_API UTurnManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTurnManagerComponent();

    // Turn control
    UFUNCTION(BlueprintCallable, Category = "Turn")
    void StartFirstTurn();

    UFUNCTION(BlueprintCallable, Category = "Turn")
    void AdvancePhase();

    UFUNCTION(BlueprintCallable, Category = "Turn")
    void EndTurn();

    // Phase queries
    UFUNCTION(BlueprintPure, Category = "Turn")
    ETurnPhase GetCurrentPhase() const { return CurrentPhase; }

    UFUNCTION(BlueprintPure, Category = "Turn")
    int32 GetCurrentPlayerIndex() const { return CurrentPlayerIndex; }

    UFUNCTION(BlueprintPure, Category = "Turn")
    int32 GetTurnNumber() const { return TurnNumber; }

    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Turn")
    FOnPhaseChanged OnPhaseChanged;

    UPROPERTY(BlueprintAssignable, Category = "Turn")
    FOnPlayerChanged OnPlayerChanged;

private:
    UPROPERTY()
    ETurnPhase CurrentPhase;

    UPROPERTY()
    int32 CurrentPlayerIndex;

    UPROPERTY()
    int32 TurnNumber;

    UPROPERTY()
    int32 TotalPlayers;

    void SetPhase(ETurnPhase NewPhase);
    void AdvancePlayer();
};
```

#### 4. URulesEngineComponent
**File: `RulesEngineComponent.h` / `RulesEngineComponent.cpp`**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RulesEngineComponent.generated.h"

class AUnit;
class ATile;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAIR_API URulesEngineComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    URulesEngineComponent();

    // Movement validation
    UFUNCTION(BlueprintPure, Category = "Rules")
    bool CanUnitMoveToTile(AUnit* Unit, FIntPoint TargetTile, int32& OutMovementCost) const;

    UFUNCTION(BlueprintPure, Category = "Rules")
    bool CanPassThroughGate(AUnit* Unit, FIntPoint GateTile) const;

    UFUNCTION(BlueprintCallable, Category = "Rules")
    int32 RollGateBreachAttempt(int32 AttackerUnitCount, int32 DefenderUnitCount) const;

    // Purchase validation
    UFUNCTION(BlueprintPure, Category = "Rules")
    bool CanAffordUnit(int32 PlayerGold, FName UnitID) const;

    UFUNCTION(BlueprintPure, Category = "Rules")
    bool CanSpawnUnitAt(FIntPoint SpawnTile, FName UnitID) const;

    // Combat validation
    UFUNCTION(BlueprintPure, Category = "Rules")
    bool CanAttackTile(AUnit* Attacker, FIntPoint TargetTile) const;

    UFUNCTION(BlueprintPure, Category = "Rules")
    bool IsRangedAttackValid(AUnit* Archer, FIntPoint TargetTile) const;

    // Mining validation
    UFUNCTION(BlueprintPure, Category = "Rules")
    bool CanMineAt(AUnit* Miner, FIntPoint Tile) const;

    // Wagon validation
    UFUNCTION(BlueprintPure, Category = "Rules")
    bool CanLoadMineral(AUnit* Wagon, FIntPoint MiningTile) const;

    UFUNCTION(BlueprintPure, Category = "Rules")
    bool CanUnloadMineral(AUnit* Wagon, FIntPoint BaseTile) const;

    // Special abilities
    UFUNCTION(BlueprintPure, Category = "Rules")
    bool HasAbility(AUnit* Unit, FName AbilityName) const;

    UFUNCTION(BlueprintCallable, Category = "Rules")
    bool CanUseAbility(AUnit* Unit, FName AbilityName) const;

    // Victory checks
    UFUNCTION(BlueprintPure, Category = "Rules")
    bool IsPlayerEliminated(int32 PlayerIndex) const;

    UFUNCTION(BlueprintPure, Category = "Rules")
    bool HasPlayerWon(int32 PlayerIndex) const;
};
```

#### 5. UCombatResolverComponent
**File: `CombatResolverComponent.h` / `CombatResolverComponent.cpp`**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatResolverComponent.generated.h"

class AUnit;
class AMonster;

USTRUCT(BlueprintType)
struct FCombatResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    TArray<AUnit*> AttackerCasualties;

    UPROPERTY(BlueprintReadOnly)
    TArray<AUnit*> DefenderCasualties;

    UPROPERTY(BlueprintReadOnly)
    bool bAttackerVictory;

    UPROPERTY(BlueprintReadOnly)
    bool bDefenderRetreated;

    UPROPERTY(BlueprintReadOnly)
    TArray<int32> AttackerDiceRolls;

    UPROPERTY(BlueprintReadOnly)
    TArray<int32> DefenderDiceRolls;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAIR_API UCombatResolverComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCombatResolverComponent();

    // Combat initiation
    UFUNCTION(BlueprintCallable, Category = "Combat")
    FCombatResult ResolveCombat(TArray<AUnit*> Attackers, TArray<AUnit*> Defenders);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    FCombatResult ResolveCombatVsMonster(TArray<AUnit*> Attackers, AMonster* Monster);

    // Dice rolling
    UFUNCTION(BlueprintCallable, Category = "Combat")
    TArray<int32> RollDice(int32 NumDice) const;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    int32 CountHits(TArray<int32> DiceRolls, TArray<int32> HitValues) const;

    // Special combat rules
    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool RollDefenceSave(AUnit* Knight) const;  // Knight ability

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool ResolveKamikazeAttack(AUnit* Saboteur, TArray<AUnit*> EnemiesInTile) const;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    TArray<AUnit*> ResolveRangedAttack(AUnit* Archer, FIntPoint TargetTile) const;

    // Combat queries
    UFUNCTION(BlueprintPure, Category = "Combat")
    int32 CalculateCombatStrength(TArray<AUnit*> Units) const;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TSubclassOf<class UUserWidget> CombatLogWidgetClass;

    void ApplyDamage(AUnit* Unit, int32 Hits);
    void ApplyDamageToMonster(AMonster* Monster, int32 Hits);
    bool IsUnitDestroyed(AUnit* Unit) const;
    void LogCombatEvent(const FString& EventText);
};
```

#### 6. UMiningSystemComponent
**File: `MiningSystemComponent.h` / `MiningSystemComponent.cpp`**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MiningSystemComponent.generated.h"

class AUnit;
class ATile;

USTRUCT(BlueprintType)
struct FMiningResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    EMiningOutcome OutcomeType;

    UPROPERTY(BlueprintReadOnly)
    FName OutcomeID;

    UPROPERTY(BlueprintReadOnly)
    int32 GoldValue;

    UPROPERTY(BlueprintReadOnly)
    bool bSpawnedMonster;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAIR_API UMiningSystemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UMiningSystemComponent();

    virtual void BeginPlay() override;

    // Mining actions
    UFUNCTION(BlueprintCallable, Category = "Mining")
    FMiningResult DrawMiningCard(AUnit* Miner, ATile* MiningTile);

    UFUNCTION(BlueprintCallable, Category = "Mining")
    void PlaceMineralOnTile(FName MineralID, ATile* Tile);

    UFUNCTION(BlueprintCallable, Category = "Mining")
    void MarkTileAsMinedOut(ATile* Tile);

    UFUNCTION(BlueprintCallable, Category = "Mining")
    bool TryRemoveMinedOut(AUnit* MasterMiner, ATile* Tile);  // Master Miner ability

    // Deck management
    UFUNCTION(BlueprintCallable, Category = "Mining")
    void ShuffleMiningDeck();

    UFUNCTION(BlueprintPure, Category = "Mining")
    int32 GetRemainingCards() const;

private:
    UPROPERTY()
    TArray<FMiningCardData> MiningDeck;

    UPROPERTY()
    int32 CurrentCardIndex;

    void InitializeDeck();
    FMiningCardData DrawCard();
    void ResolveMiningOutcome(FMiningCardData CardData, AUnit* Miner, ATile* Tile, FMiningResult& OutResult);
    void SpawnMonster(FName MonsterID, ATile* Tile);
    void ApplyTreasure(FName TreasureID, AUnit* Finder);
};
```

#### 7. ATile
**File: `Tile.h` / `Tile.cpp`**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

class AUnit;

USTRUCT(BlueprintType)
struct FTileSubSlot
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    bool bIsOccupied;

    UPROPERTY(BlueprintReadWrite)
    AUnit* OccupyingUnit;

    FTileSubSlot()
        : bIsOccupied(false), OccupyingUnit(nullptr)
    {
    }
};

UCLASS()
class LAIR_API ATile : public AActor
{
    GENERATED_BODY()

public:
    ATile();

    virtual void BeginPlay() override;

    // Grid position
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
    FIntPoint GridCoord;

    // Tile type
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
    FName TileTypeID;

    // Sub-slots (4 quarters of the tile)
    UPROPERTY(BlueprintReadOnly, Category = "Tile")
    TArray<FTileSubSlot> SubSlots;  // Always size 4

    // Mining state
    UPROPERTY(BlueprintReadWrite, Category = "Tile")
    bool bHasBeenMined;

    UPROPERTY(BlueprintReadWrite, Category = "Tile")
    bool bIsMinedOut;

    UPROPERTY(BlueprintReadWrite, Category = "Tile")
    FName MineralOnTile;  // Empty if no mineral

    // Gate state
    UPROPERTY(BlueprintReadWrite, Category = "Tile")
    bool bHasGate;

    UPROPERTY(BlueprintReadWrite, Category = "Tile")
    bool bGateIsDefended;

    // Mystery tile state
    UPROPERTY(BlueprintReadWrite, Category = "Tile")
    bool bIsMysteryTile;

    UPROPERTY(BlueprintReadWrite, Category = "Tile")
    bool bMysteryRevealed;

    UPROPERTY(BlueprintReadWrite, Category = "Tile")
    FName MysteryTileID;

    // Outpost state
    UPROPERTY(BlueprintReadWrite, Category = "Tile")
    bool bIsOutpost;

    UPROPERTY(BlueprintReadWrite, Category = "Tile")
    bool bOutpostExplored;

    UPROPERTY(BlueprintReadWrite, Category = "Tile")
    int32 OutpostOwnerIndex;  // -1 if unowned

    // Visual components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* TileMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MineralMesh;  // Shown when mineral is present

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MinedOutIndicator;

    // Public API
    UFUNCTION(BlueprintCallable, Category = "Tile")
    bool CanPlaceUnit(int32 SubSlotSize) const;

    UFUNCTION(BlueprintCallable, Category = "Tile")
    int32 GetAvailableSubSlots() const;

    UFUNCTION(BlueprintCallable, Category = "Tile")
    bool PlaceUnitInSubSlot(AUnit* Unit, int32 SubSlotIndex);

    UFUNCTION(BlueprintCallable, Category = "Tile")
    bool RemoveUnitFromSubSlot(AUnit* Unit);

    UFUNCTION(BlueprintPure, Category = "Tile")
    TArray<AUnit*> GetAllUnitsOnTile() const;

    UFUNCTION(BlueprintCallable, Category = "Tile")
    void SetHighlight(bool bHighlighted, FLinearColor Color = FLinearColor::Green);

protected:
    UPROPERTY()
    UMaterialInstanceDynamic* DynamicMaterial;

    void UpdateVisuals();
};
```

#### 8. AUnit
**File: `Unit.h` / `Unit.cpp`**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Unit.generated.h"

class ATile;

UCLASS()
class LAIR_API AUnit : public AActor
{
    GENERATED_BODY()

public:
    AUnit();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // Unit data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
    FName UnitID;  // References DT_Units

    UPROPERTY(BlueprintReadOnly, Category = "Unit")
    FUnitData CachedUnitData;

    // Ownership
    UPROPERTY(BlueprintReadWrite, Category = "Unit")
    int32 OwnerPlayerIndex;

    // Position
    UPROPERTY(BlueprintReadWrite, Category = "Unit")
    ATile* CurrentTile;

    UPROPERTY(BlueprintReadWrite, Category = "Unit")
    int32 SubSlotIndex;

    // State
    UPROPERTY(BlueprintReadWrite, Category = "Unit")
    int32 CurrentHP;

    UPROPERTY(BlueprintReadWrite, Category = "Unit")
    int32 RemainingMovement;

    UPROPERTY(BlueprintReadWrite, Category = "Unit")
    bool bHasActedThisTurn;

    // Wagon-specific
    UPROPERTY(BlueprintReadWrite, Category = "Unit")
    bool bIsCarryingMineral;

    UPROPERTY(BlueprintReadWrite, Category = "Unit")
    FName CarriedMineralID;

    // Treasure-specific
    UPROPERTY(BlueprintReadWrite, Category = "Unit")
    TArray<FName> CarriedTreasures;

    // Visual components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* UnitMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWidgetComponent* HealthBarWidget;

    // Public API
    UFUNCTION(BlueprintCallable, Category = "Unit")
    void InitializeFromDataTable(FName InUnitID, int32 InOwnerIndex);

    UFUNCTION(BlueprintCallable, Category = "Unit")
    void ResetMovement();

    UFUNCTION(BlueprintCallable, Category = "Unit")
    void TakeDamage(int32 Hits);

    UFUNCTION(BlueprintPure, Category = "Unit")
    bool IsDestroyed() const;

    UFUNCTION(BlueprintCallable, Category = "Unit")
    void SetSelected(bool bSelected);

    UFUNCTION(BlueprintCallable, Category = "Unit")
    void MoveTo(ATile* TargetTile, int32 TargetSubSlot);

protected:
    UPROPERTY()
    UMaterialInstanceDynamic* DynamicMaterial;

    void UpdateVisuals();
    void LoadDataFromTable();
};
```

#### 9. AMonster
**File: `Monster.h` / `Monster.cpp`**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Monster.generated.h"

class ATile;

UCLASS()
class LAIR_API AMonster : public AActor
{
    GENERATED_BODY()

public:
    AMonster();

    virtual void BeginPlay() override;

    // Monster data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    FName MonsterID;  // References DT_Monsters

    UPROPERTY(BlueprintReadOnly, Category = "Monster")
    FMonsterData CachedMonsterData;

    // Position
    UPROPERTY(BlueprintReadWrite, Category = "Monster")
    ATile* CurrentTile;

    // State
    UPROPERTY(BlueprintReadWrite, Category = "Monster")
    int32 CurrentHP;

    UPROPERTY(BlueprintReadWrite, Category = "Monster")
    bool bIsActive;

    // Visual components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MonsterMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWidgetComponent* HealthBarWidget;

    // Public API
    UFUNCTION(BlueprintCallable, Category = "Monster")
    void InitializeFromDataTable(FName InMonsterID, ATile* SpawnTile);

    UFUNCTION(BlueprintCallable, Category = "Monster")
    void TakeDamage(int32 Hits);

    UFUNCTION(BlueprintCallable, Category = "Monster")
    void Regenerate();

    UFUNCTION(BlueprintPure, Category = "Monster")
    bool IsDestroyed() const;

    UFUNCTION(BlueprintCallable, Category = "Monster")
    TArray<int32> RollAttack() const;

protected:
    void LoadDataFromTable();
    void UpdateVisuals();
};
```

---

## Blueprint Systems

### Widget Blueprints (UI)

#### 1. WBP_MainHUD (Main game HUD)
**Components:**
- Player info panel (gold, faction, current phase)
- Turn indicator (whose turn, turn number)
- Phase indicator (Purchase/Mining/Movement)
- Unit selection panel (shows selected unit stats)
- Action buttons (End Turn, Retreat, etc.)
- Combat log (scrolling text log)
- Victory condition tracker

**Blueprint Events:**
- OnPhaseChanged → Update phase indicator
- OnPlayerChanged → Update player info
- OnUnitSelected → Populate unit panel
- OnCombatEvent → Add to combat log

#### 2. WBP_PurchaseMenu (Purchase phase UI)
**Components:**
- Grid of purchasable units (icons, names, costs)
- Gold display
- "Place at Base" / "Place at Outpost" toggle
- Buy button
- Close button

**Blueprint Logic:**
- Query available units for current player faction
- Check gold affordability
- Call GameMode → Purchase unit
- Visual feedback on successful purchase

#### 3. WBP_TileInfo (Tile hover tooltip)
**Components:**
- Tile type label
- Occupants list (units, monsters)
- Mining status indicator
- Gate/outpost status

**Blueprint Logic:**
- Updates on mouse hover over tiles
- Queries tile data from BoardSystem

#### 4. WBP_CombatDialog (Combat resolution popup)
**Components:**
- Attacker unit list (with HP bars)
- Defender unit list (with HP bars)
- Dice roll animation area
- Combat log text
- "Continue" / "Retreat" buttons

**Blueprint Logic:**
- Triggered when combat initiates
- Displays real-time dice rolls and damage
- Allows player to retreat mid-combat

#### 5. WBP_MiningResultPopup (Mining card reveal)
**Components:**
- Card art display
- Outcome text (e.g., "Iron Ore - 50 GP")
- Monster spawn notification (if applicable)
- "Continue" button

**Blueprint Logic:**
- Shows mining card result
- Spawns monster if needed
- Updates tile state

#### 6. WBP_VictoryScreen (End game screen)
**Components:**
- Victory message (e.g., "Blue Player Wins!")
- Victory condition achieved text
- Final gold totals
- "New Game" / "Quit" buttons

**Blueprint Logic:**
- Triggered when victory condition met
- Displays winner and reason

### Camera Blueprint (BP_CameraController)
**Components:**
- Spring arm component
- Camera component
- Mouse/keyboard input bindings

**Blueprint Logic:**
- WASD/Arrow keys → Pan camera
- Mouse scroll → Zoom in/out
- Q/E keys → Rotate camera
- Click tile → Focus on tile
- Click unit → Focus on unit

### Utility Blueprints

#### BP_TileHighlight (Visual feedback)
**Purpose:** Spawned dynamically to highlight valid movement tiles, attack range, etc.

**Components:**
- Decal component (green for valid, red for invalid, yellow for attack range)

**Blueprint Logic:**
- Spawned by player controller
- Positioned at tile world location
- Destroyed when no longer needed

---

## Game State Management

### Save/Load System (Future Phase)

**FSaveGameData Structure:**
```cpp
USTRUCT(BlueprintType)
struct FSaveGameData
{
    GENERATED_BODY()

    UPROPERTY()
    int32 SaveVersion;

    UPROPERTY()
    int32 TurnNumber;

    UPROPERTY()
    int32 CurrentPlayerIndex;

    UPROPERTY()
    ETurnPhase CurrentPhase;

    UPROPERTY()
    TArray<FPlayerSaveData> Players;

    UPROPERTY()
    TArray<FTileSaveData> Tiles;

    UPROPERTY()
    TArray<FUnitSaveData> Units;

    UPROPERTY()
    TArray<FMonsterSaveData> Monsters;

    UPROPERTY()
    FDateTime SaveTimestamp;
};
```

### Networked Multiplayer State (Future Phase)

**Replication Strategy:**
- Server authoritative (all game logic runs on server)
- Clients send input (move unit, attack, purchase, etc.)
- Server validates and broadcasts state changes
- Use Unreal's replication system for:
  - Player gold
  - Unit positions
  - Tile states
  - Turn/phase changes

**Key Replicated Classes:**
- `ALairGameMode` (server only)
- `ALairPlayerState` (replicated to all)
- `AUnit` (replicated to all, movement smoothing)
- `ATile` (replicated to all, state changes only)

---

## Implementation Phases

### Phase 1: Foundation (Weeks 1-2)
**Goal:** Core systems functional, local hotseat playable

**Deliverables:**
1. **C++ Core Systems**
   - ALairGameMode with component architecture
   - UBoardSystemComponent (grid creation, tile access)
   - UTurnManagerComponent (phase sequencing)
   - URulesEngineComponent (basic validation)
   - Data table integration

2. **Data Tables**
   - Create all 9 data tables with sample data
   - Import CSV/JSON into Unreal

3. **Basic Actors**
   - ATile (visual mesh, sub-slot logic)
   - AUnit (spawning, placement, visual)
   - Test with 2-3 unit types

4. **Blueprint UI**
   - WBP_MainHUD (minimal version)
   - WBP_PurchaseMenu (minimal version)
   - Camera controller

5. **Test Map**
   - Small 10x10 grid
   - 2 player bases (one at each corner)
   - All other tiles marked as "Empty" type
   - Manual testing of purchase → placement → turn advancement

**Phase 1 Scope Note:** Mining, combat, and monsters are NOT implemented in Phase 1. Tiles may be tagged with types (e.g., "Mining") in the data table for future use, but no mining interactions are functional.

**Success Criteria:**
- Can start game with 2 players
- Can purchase units and place at base
- Units appear on board visually
- Can advance turn phases manually

---

### Phase 2: Mining & Combat (Weeks 3-4)
**Goal:** Mining system, combat resolution, monsters

**Deliverables:**
1. **Mining System**
   - UMiningSystemComponent (deck shuffling, card drawing)
   - Mining card outcomes (minerals, treasures, monsters, mined out)
   - Mineral placement on tiles
   - Mined out markers

2. **Combat System**
   - UCombatResolverComponent (dice rolling, damage)
   - Combat vs units
   - Combat vs monsters
   - WBP_CombatDialog (visual combat log)

3. **Monster AI**
   - AMonster actor (spawning, HP, regeneration)
   - Monster attack logic
   - Monster persistence on tiles

4. **Wagon System**
   - Mineral loading/unloading
   - Transport to base
   - Gold conversion

5. **UI Updates**
   - WBP_MiningResultPopup
   - WBP_CombatDialog
   - Combat log in main HUD

**Success Criteria:**
- Can mine tiles and draw cards
- Minerals spawn correctly
- Wagons can load and transport minerals
- Combat resolves with dice rolls
- Monsters spawn and regenerate

---

### Phase 3: Full Game Loop (Weeks 5-6)
**Goal:** All unit types, special abilities, victory conditions

**Deliverables:**
1. **All Unit Types**
   - Implement all 8 elite units
   - Special ability logic (Infiltrate, Defence, Kamikaze, etc.)
   - Master Miner (remove mined out)
   - Archers (ranged attack)

2. **Mystery Tiles**
   - Tile placement during setup
   - Trap resolution (cave-in, pit trap, bear trap)
   - Treasure outcomes
   - Monster spawns

3. **Gates & Outposts**
   - Gate breach mechanics
   - Outpost exploration
   - Outpost ownership

4. **Dragon's Lair**
   - Dragon combat (multi-gate attack)
   - Dragon treasure victory condition

5. **Victory System**
   - UVictoryManagerComponent
   - Check all 3 victory conditions
   - WBP_VictoryScreen

6. **Polish**
   - Sound effects (dice rolls, combat, mining)
   - Visual effects (explosions, treasure glow)
   - Animation (unit movement, combat)

**Success Criteria:**
- Full 6-player game playable
- All victory conditions functional
- All unit abilities working
- Mystery tiles and outposts explored

---

### Phase 4: Polish & Balance (Week 7)
**Goal:** Bug fixes, balance tweaks, UX improvements

**Deliverables:**
1. **Balance Pass**
   - Adjust unit costs, HP, attack values
   - Tweak mining card distribution
   - Test victory condition balance

2. **UX Polish**
   - Tooltips for all buttons
   - Tutorial overlay (optional)
   - Better visual feedback (tile highlights, selection glow)

3. **Performance**
   - Optimize pathfinding
   - LOD for unit meshes
   - Memory profiling

4. **Packaging**
   - Build Windows standalone executable
   - Test on clean machine
   - Create installer

**Success Criteria:**
- Game runs at 60 FPS on mid-tier PC
- No crashes or major bugs
- Balanced gameplay (tested with 4-6 games)

---

### Phase 5 (Future): Online Multiplayer
**Goal:** Async turn-based multiplayer

**Deliverables:**
1. **Server Architecture**
   - Dedicated server or Epic Online Services
   - Lobby system
   - Matchmaking

2. **Replication**
   - Replicate all game state
   - Client prediction for unit movement
   - Server validation

3. **Async Support**
   - Email notifications for turn
   - Save/load game state
   - Reconnect handling

4. **Testing**
   - LAN testing
   - Online playtesting

---

## File Structure

### Unreal Project Structure

```
Lair/
├── Config/
│   └── DefaultGame.ini (project settings)
├── Content/
│   ├── Blueprints/
│   │   ├── GameMode/
│   │   │   └── BP_LairGameMode.uasset (inherits ALairGameMode)
│   │   ├── UI/
│   │   │   ├── WBP_MainHUD.uasset
│   │   │   ├── WBP_PurchaseMenu.uasset
│   │   │   ├── WBP_CombatDialog.uasset
│   │   │   ├── WBP_MiningResultPopup.uasset
│   │   │   ├── WBP_TileInfo.uasset
│   │   │   └── WBP_VictoryScreen.uasset
│   │   ├── Camera/
│   │   │   └── BP_CameraController.uasset
│   │   ├── Actors/
│   │   │   ├── BP_Tile.uasset (inherits ATile)
│   │   │   ├── BP_Unit.uasset (inherits AUnit)
│   │   │   └── BP_Monster.uasset (inherits AMonster)
│   │   └── Utility/
│   │       └── BP_TileHighlight.uasset
│   ├── Data/
│   │   ├── DT_Units.uasset
│   │   ├── DT_Monsters.uasset
│   │   ├── DT_MiningCards.uasset
│   │   ├── DT_Minerals.uasset
│   │   ├── DT_Treasures.uasset
│   │   ├── DT_MysteryTiles.uasset
│   │   ├── DT_Factions.uasset
│   │   ├── DT_TileTypes.uasset
│   │   └── DT_VictoryConditions.uasset
│   ├── Maps/
│   │   ├── MainMenu.umap (future: main menu level)
│   │   └── GameBoard.umap (main game level)
│   ├── Materials/
│   │   ├── M_Tile_Base.uasset
│   │   ├── M_Unit_Base.uasset
│   │   ├── M_Monster_Base.uasset
│   │   └── M_Highlight.uasset
│   ├── Meshes/
│   │   ├── Tiles/
│   │   │   ├── SM_Tile_Empty.uasset
│   │   │   ├── SM_Tile_Mining.uasset
│   │   │   ├── SM_Tile_Gate.uasset
│   │   │   └── SM_Tile_Outpost.uasset
│   │   ├── Units/
│   │   │   ├── SM_Footman.uasset
│   │   │   ├── SM_Miner.uasset
│   │   │   ├── SM_Wagon.uasset
│   │   │   └── [other units]
│   │   └── Monsters/
│   │       ├── SM_Dragon.uasset
│   │       ├── SM_Giant.uasset
│   │       └── [other monsters]
│   ├── Textures/
│   │   ├── Icons/
│   │   │   ├── T_Icon_Footman.uasset
│   │   │   ├── T_Icon_Miner.uasset
│   │   │   └── [other icons]
│   │   └── UI/
│   │       └── [UI textures]
│   └── Sounds/
│       ├── SFX_DiceRoll.uasset
│       ├── SFX_Combat.uasset
│       └── SFX_Mining.uasset
├── Source/
│   └── Lair/
│       ├── Private/
│       │   ├── LairGameMode.cpp
│       │   ├── BoardSystemComponent.cpp
│       │   ├── TurnManagerComponent.cpp
│       │   ├── RulesEngineComponent.cpp
│       │   ├── MiningSystemComponent.cpp
│       │   ├── CombatResolverComponent.cpp
│       │   ├── VictoryManagerComponent.cpp
│       │   ├── Tile.cpp
│       │   ├── Unit.cpp
│       │   ├── Monster.cpp
│       │   └── LairPlayerState.cpp
│       ├── Public/
│       │   ├── LairGameMode.h
│       │   ├── BoardSystemComponent.h
│       │   ├── TurnManagerComponent.h
│       │   ├── RulesEngineComponent.h
│       │   ├── MiningSystemComponent.h
│       │   ├── CombatResolverComponent.h
│       │   ├── VictoryManagerComponent.h
│       │   ├── Tile.h
│       │   ├── Unit.h
│       │   ├── Monster.h
│       │   ├── LairPlayerState.h
│       │   └── LairDataStructs.h (all FStructs and Enums)
│       ├── Lair.Build.cs (module dependencies)
│       └── Lair.h (module header)
└── Lair.uproject (project file)
```

---

## Testing Strategy

**Note:** This section describes testing strategies for all phases. Examples referencing combat, mining, and monster systems are for Phase 2+ and serve as future reference only. For Phase 1 testing, see the 10 acceptance tests in Implementation_Plan_Phase1_v1.5.md.

### Unit Tests (C++)
**Scope:** Core logic functions

**Examples:**
- `UBoardSystemComponent::IsValidGridCoord()`
- `URulesEngineComponent::CanUnitMoveToTile()`
- `UCombatResolverComponent::RollDice()`
- `UMiningSystemComponent::DrawCard()`

**Implementation:**
- Use Unreal's Automation Testing framework
- Create `LairTests` module
- Run tests in editor with `Session Frontend`

### Integration Tests (Blueprint)
**Scope:** Multi-system interactions

**Examples:**
- Purchase unit → Verify gold deduction → Verify unit spawned at base
- Mine tile → Draw card → Verify mineral placed → Load wagon → Transport to base → Verify gold added
- Attack enemy → Roll dice → Verify damage → Verify unit destroyed

**Implementation:**
- Create test maps with scripted scenarios
- Use Blueprint testing nodes

### Playtesting
**Scope:** Full game balance and UX

**Process:**
1. Internal testing (dev team, 4-6 games)
2. Identify balance issues (e.g., "Assassins too strong")
3. Adjust data tables
4. Retest

**Metrics:**
- Average game length (target: 30-60 minutes)
- Victory condition distribution (aim for 33/33/33 split)
- Player elimination rate (should be rare in 2-player, common in 6-player)

---

## Technical Requirements

### Development Environment
- **Engine:** Unreal Engine 5.3+ (latest stable)
- **IDE:** Visual Studio 2022 (for C++ development)
- **Source Control:** Git with LFS (Large File Storage for assets)
- **OS:** Windows 10/11 (primary), Mac/Linux (secondary)

### Target Hardware (MVP)
- **CPU:** Intel i5 / AMD Ryzen 5 (4 cores)
- **GPU:** NVIDIA GTX 1060 / AMD RX 580 (4GB VRAM)
- **RAM:** 8GB minimum, 16GB recommended
- **Storage:** 5GB disk space

### Performance Targets
- **Framerate:** 60 FPS minimum on target hardware
- **Load Time:** <10 seconds from launch to main menu
- **Turn Processing:** <1 second for AI/validation

### Code Quality Standards
- **C++ Style:** Follow Unreal Engine coding standard
- **Naming:** PascalCase for classes, camelCase for variables, UPPER_CASE for constants
- **Comments:** Document all public API functions
- **Architecture:** Single Responsibility Principle, component-based design

---

## Implementation Instructions

### ⚠️ IMPORTANT: Follow the Implementation Plan

**Do not use this document for file generation order or Phase 1 scope.**

Instead, refer to: **Implementation_Plan_Phase1_v1.5.md**

That document contains:
- ✅ Exact file generation order (10 steps, dependency-safe)
- ✅ Phase 1 scope definition (what's in, what's out)
- ✅ 10 acceptance tests (Given/When/Then format)
- ✅ Rules clarifications (movement, stacking, gates)
- ✅ Board layout data system
- ✅ Quality checklist per file

### Purpose of This Document

Use this Technical Specification as a **reference** for:
- Understanding overall system architecture
- Looking up data table structure definitions
- Understanding how systems interact across all phases
- Planning phases beyond Phase 1

### Quick Links

- **Building Phase 1:** See Implementation_Plan_Phase1_v1.5.md
- **Getting Started:** See LAIR_Quick_Start_Guide.md
- **Game Rules Reference:** See LAIR_Rules_Glossary_v1.0.md
- **Claude Code Instructions:** See Master_Build_Prompt_v3.0.md

---

## Step-by-Step Quickstart (If Starting Fresh)

### Step 1: Initialize Project
```bash
# Clone GitHub repository (if existing)
git clone <repo_url>

# Or create new UE5 project
# File → New Project → Games → Blank → C++ → "Lair"
```

### Step 2: Follow Implementation Plan
Refer to Implementation_Plan_Phase1_v1.5.md for exact file creation order.

### Step 3: Create Data Tables
1. Create struct types in C++ first
2. In Unreal Editor: Right-click → Miscellaneous → Data Table
3. Choose row struct (e.g., `FUnitData`)
4. Populate with sample data

### Step 4: Create Blueprint Assets
1. Create BP_LairGameMode (inherits ALairGameMode)
2. Create BP_Tile (inherits ATile)
3. Create BP_Unit (inherits AUnit)
4. Assign data table references in BP_LairGameMode defaults

### Step 5: Build Test Map
1. Create `GameBoard.umap`
2. Place BP_LairGameMode as default game mode
3. Use World Settings to configure board size
4. Test in PIE (Play In Editor)

### Step 6: Iterate
- Test frequently in PIE
- Commit to Git after each feature
- Use Unreal's Blueprint debugger for visual debugging

---

## Appendix: Key Enums

```cpp
// LairDataStructs.h

UENUM(BlueprintType)
enum class EUnitType : uint8
{
    Miner UMETA(DisplayName = "Miner"),
    Wagon UMETA(DisplayName = "Wagon"),
    Footman UMETA(DisplayName = "Footman"),
    Elite UMETA(DisplayName = "Elite")
};

UENUM(BlueprintType)
enum class EArmyFaction : uint8
{
    Neutral UMETA(DisplayName = "Neutral"),
    Dwarves UMETA(DisplayName = "Dwarves"),
    Humans UMETA(DisplayName = "Humans"),
    Gnomes UMETA(DisplayName = "Gnomes"),
    Orcs UMETA(DisplayName = "Orcs"),
    DarkElves UMETA(DisplayName = "Dark Elves"),
    Goblins UMETA(DisplayName = "Goblins")
};

UENUM(BlueprintType)
enum class EMiningOutcome : uint8
{
    Mineral UMETA(DisplayName = "Mineral"),
    Treasure UMETA(DisplayName = "Treasure"),
    Monster UMETA(DisplayName = "Monster"),
    MinedOut UMETA(DisplayName = "Mined Out"),
    DigDeeper UMETA(DisplayName = "Dig Deeper")
};

UENUM(BlueprintType)
enum class ETreasureEffect : uint8
{
    ImmediateGold UMETA(DisplayName = "Immediate Gold"),
    SpawnUnits UMETA(DisplayName = "Spawn Units"),
    AddWagon UMETA(DisplayName = "Add Wagon"),
    OneTimeUse UMETA(DisplayName = "One-Time Use"),
    MultiUse UMETA(DisplayName = "Multi-Use")
};

UENUM(BlueprintType)
enum class EMysteryTileType : uint8
{
    Trap UMETA(DisplayName = "Trap"),
    Treasure UMETA(DisplayName = "Treasure"),
    Monster UMETA(DisplayName = "Monster"),
    LostUnit UMETA(DisplayName = "Lost Unit")
};

UENUM(BlueprintType)
enum class EVictoryType : uint8
{
    GoldThreshold UMETA(DisplayName = "Gold Threshold"),
    Elimination UMETA(DisplayName = "Elimination"),
    DragonTreasure UMETA(DisplayName = "Dragon Treasure")
};

UENUM(BlueprintType)
enum class ETurnPhase : uint8
{
    Purchase UMETA(DisplayName = "Purchase Phase"),
    Mining UMETA(DisplayName = "Mining Phase"),
    MovementCombat UMETA(DisplayName = "Movement & Combat Phase"),
    EndTurn UMETA(DisplayName = "End Turn")
};
```

---

## Appendix: Key Constants

```cpp
// LairGameConstants.h

namespace LairConstants
{
    // Grid
    constexpr int32 DEFAULT_BOARD_SIZE_X = 20;
    constexpr int32 DEFAULT_BOARD_SIZE_Y = 20;
    constexpr int32 TILE_SUB_SLOTS = 4;
    constexpr float TILE_WORLD_SIZE = 100.0f;  // cm

    // Movement
    constexpr int32 ORTHOGONAL_MOVE_COST = 1;
    constexpr int32 DIAGONAL_MOVE_COST = 2;

    // Starting values
    constexpr int32 STARTING_GOLD = 200;
    constexpr int32 STARTING_MINERS = 1;
    constexpr int32 STARTING_WAGONS = 1;
    constexpr int32 STARTING_FOOTMEN = 1;

    // Victory
    constexpr int32 GOLD_VICTORY_THRESHOLD = 3000;

    // Combat
    constexpr int32 DICE_SIDES = 6;
    constexpr int32 MAX_DICE_PER_UNIT = 3;

    // Mining
    constexpr int32 TOTAL_MINING_CARDS = 120;
    constexpr int32 TOTAL_MYSTERY_TILES = 16;

    // Ranged attack
    constexpr int32 ARCHER_RANGE = 4;  // tiles

    // Temple of Baladin
    constexpr int32 TEMPLE_MAX_TURNS = 1;
}
```

---

## Summary

This specification provides:
1. **Complete data-driven architecture** - All gameplay values in Unreal Data Tables
2. **C++ class hierarchy** - 9 core classes with full API definitions
3. **Blueprint systems** - 6 widget blueprints for UI
4. **Phased implementation plan** - 7 weeks to MVP
5. **File structure** - Organized project layout
6. **Testing strategy** - Unit tests, integration tests, playtesting
7. **Deployment path** - Windows standalone executable

Claude Code can now:
- Generate C++ classes from the provided headers
- Create data tables with sample data
- Build Blueprint assets
- Implement systems phase-by-phase
- Test iteratively in Unreal Editor

**Recommended Starting Point:**
Begin with Phase 1, implementing `LairGameMode`, `BoardSystemComponent`, `Tile`, and `Unit` classes. Get a basic 2-player purchase → placement flow working, then build from there.

---

**End of Specification v1.0**
