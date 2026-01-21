# LAIR - Rules & Terms Glossary
**Comprehensive Reference for All Contributors**

---

## Purpose

This glossary defines all technical and game-specific terms used in LAIR documentation. Useful for:
- Non-technical contributors (designers, artists, testers)
- New developers joining the project
- Quick reference during discussions
- Avoiding ambiguity in specifications

---

## Table of Contents

- [Game Concepts](#game-concepts)
- [Board & Layout Terms](#board--layout-terms)
- [Unit & Piece Terms](#unit--piece-terms)
- [Movement & Combat Terms](#movement--combat-terms)
- [Technical Terms](#technical-terms)
- [Development Terms](#development-terms)
- [Phase-Specific Terms](#phase-specific-terms)

---

## Game Concepts

### Phase
A segment of a player's turn. The full LAIR game has three phases per turn:
1. **Purchase Phase** - Buy new units
2. **Mining Phase** - Extract resources (Phase 2+)
3. **Movement Phase** - Move units (Phase 2+)

**Phase 1 MVP Implementation:** Only the Purchase Phase is fully implemented. Mining and Movement phases exist as stubs for turn sequencing but have no gameplay mechanics yet.

**Example:** "During the Purchase Phase, you can buy a Footman for 50 gold."

### Turn
One complete cycle through all phases for one player. After all phases complete, the next player takes their turn.

**Full Game:** "Player 1's turn consists of Purchase → Mining → Movement, then Player 2's turn begins."

**Phase 1 MVP:** "Player 1 purchases units, then advances to next player (Mining/Movement are skipped or auto-advanced)."

### Hotseat
Multiple players taking turns on the same computer, passing control between them. No online connection required.

**Example:** "Phase 1 is 2-player hotseat only - both players sit at one PC."

### Victory Condition
A way to win the game. LAIR has three:
1. Accumulate 3000 gold
2. Eliminate all other players
3. Retrieve the dragon treasure and return it to base

**Status in Phase 1:** Not implemented (infinite play for testing)

---

## Board & Layout Terms

### Board
The game playing area. A grid of tiles where units move and interact.

**Phase 1 Board:** 10x10 grid (100 tiles total)

### Grid
The coordinate system for the board. Uses (X, Y) coordinates starting from (0, 0).

**Example:** Player 1's base is at grid coordinate (0, 0), meaning column 0, row 0.

### Tile
One square on the board grid. The basic unit of the board system. Each tile can hold units.

**Technical:** Represented by the `ATile` actor class.

### Tile Type
The category of a tile, defining what can happen there.

**Phase 1 Types:**
- **Empty** - Standard walkable tile
- **PlayerBase** - Where units spawn

**Future Types:** Mining, Gate, Outpost, DragonLair, Temple

### Sub-Slot
One quarter of a tile. Each tile has 4 sub-slots arranged in quadrants (NW, NE, SW, SE).

**Why it matters:** Units occupy sub-slots. A normal unit takes 1 sub-slot, a wagon takes 2.

**Example:** A tile with 3 footmen (1 sub-slot each) has 1 sub-slot remaining, so a wagon (needs 2) cannot be placed there.

### Sub-Slot Size
How many sub-slots a unit occupies:
- **Size 1:** Normal units (Miner, Footman)
- **Size 2:** Wagons

**Rule:** Total sub-slots used on a tile cannot exceed 4.

### Player Base
The starting tile where a player spawns units. Each player has one base tile.

**Phase 1:** 
- Player 1 base at (0, 0) - blue
- Player 2 base at (9, 9) - red

### Layout Row
A single row in the DT_BoardLayout data table, defining one tile's properties (coordinate, type, special features).

**Example:** Row with GridCoord=(0,0), TileTypeID=PlayerBase, PlayerBaseIndex=0 defines Player 1's base.

### Board Layout Data
A Data Table (DT_BoardLayout_*) that defines the entire board configuration without hardcoding.

**Phase 1:** Uses DT_BoardLayout_Test10x10 (simple 10x10 grid)  
**Future:** Will use DT_BoardLayout_Rulebook20x20 (full game board with dragon lair, gates, outposts)

---

## Unit & Piece Terms

### Unit
A game piece representing a soldier, miner, or wagon. Controlled by a player, placed on tiles.

**Technical:** Represented by the `AUnit` actor class.

### Unit Type
The specific kind of unit (Miner, Footman, Wagon, etc.). Defines stats, cost, and abilities.

**Phase 1 Units:**
- **Miner** - 40gp, 4 movement
- **Footman** - 50gp, 5 movement
- **Wagon** - 70gp, 6 movement

### Faction
The player's team/army. Each faction has a unique color and (in later phases) special units.

**Phase 1 Factions:**
- **Player1** - Blue
- **Player2** - Red

**Future:** 6 factions (Dwarves, Humans, Gnomes, Orcs, Dark Elves, Goblins)

### Elite Unit
Faction-specific unit with special abilities (Phase 3+).

**Examples (future):**
- **Dark Elf Assassin** - Can infiltrate through enemy units
- **Human Knight** - Strong defense
- **Goblin Saboteur** - Explosive kamikaze attack

**Status in Phase 1:** Not implemented

### Wagon
A special unit that transports minerals from mines to bases. Occupies 2 sub-slots.

**Phase 1:** No minerals to transport yet, but wagon placement logic is implemented.

### Monster
NPC enemy that guards mines or appears in mystery tiles (Phase 2+).

**Examples (future):** Dragon, Giant, Banshee, Zombies, Skeletons

**Status in Phase 1:** Not implemented

### HP (Hit Points)
Amount of damage a unit can take before being destroyed. Most units have 1 HP.

**Phase 1:** HP tracking exists but no combat, so units never take damage.

---

## Movement & Combat Terms

### Movement Points
How many tiles a unit can move per turn. Each unit type has different movement.

**Phase 1 Movement:**
- Miner: 4 points
- Footman: 5 points
- Wagon: 6 points

### Orthogonal Movement
Moving in cardinal directions: North, South, East, West.

**Cost:** 1 movement point per tile

**Example:** Moving from (5, 5) to (5, 6) costs 1 point.

### Diagonal Movement
Moving in diagonal directions: NE, NW, SE, SW.

**Cost:** 2 movement points per tile

**Example:** Moving from (5, 5) to (6, 6) costs 2 points.

### Corner Blocking
When diagonal movement is blocked by units in adjacent corners.

**LAIR Rule:** Corner blocking does NOT exist. Diagonals are always allowed (if you have movement points).

### Stacking
Placing multiple units on the same tile.

**Rules:**
- **Friendly stacking:** Allowed (up to 4 sub-slots)
- **Enemy stacking:** NOT allowed (triggers combat in Phase 2)

### Passing Through
Moving onto a tile and continuing movement in the same turn.

**Rules:**
- **Friendly units:** Can pass through
- **Enemy units:** CANNOT pass through

### Gate
A defensive structure that requires a breach roll to pass (Phase 2+).

**Status in Phase 1:** Not implemented, but breach rules are documented.

### Gate Breach
Attempting to move through an enemy-defended gate by rolling dice.

**Success depends on unit counts:**
- Defender has more units → Roll 5,6 to breach
- Equal units → Roll 4,5,6 to breach
- Attacker has more units → Roll 3,4,5,6 to breach

**Status in Phase 1:** Rules defined but not implemented.

---

## Technical Terms

### Data Table
An Unreal Engine asset that stores structured data (like a spreadsheet). Used for unit stats, tile types, board layouts, etc.

**Format:** Rows and columns, where each row is an item (e.g., one unit type).

**Phase 1 Data Tables:**
- DT_Units
- DT_TileTypes
- DT_Factions
- DT_BoardLayout_Test10x10

### Row Name
The unique identifier for a row in a Data Table.

**Example:** In DT_Units, row names are "Miner", "Footman", "Wagon"

**Important:** Row names are case-sensitive!

### Struct
A C++ data structure that defines the format of a Data Table row.

**Example:** `FUnitData` struct defines what columns exist in DT_Units (Cost, MovementPoints, HP, etc.)

### Blueprint (BP)
Unreal Engine's visual scripting system. Used for UI, simple logic, and integrating C++ classes.

**Phase 1 Blueprints:**
- BP_LairGameMode (inherits from C++ class)
- BP_Tile (inherits from C++ class)
- BP_Unit (inherits from C++ class)

### PIE (Play In Editor)
Testing the game inside the Unreal Editor without packaging an .exe.

**Phase 1 Goal:** Game must be playable in PIE.

### Component
A reusable piece of functionality attached to an actor. LAIR uses components for systems like BoardSystem, TurnManager, RulesEngine.

**Example:** `UBoardSystemComponent` manages the grid of tiles.

### Actor
The base class for objects in the Unreal world. Tiles and Units are actors.

**Naming:** Actor classes start with `A` (e.g., `ATile`, `AUnit`)

### Game Mode
The central authority that defines game rules and owns all systems.

**LAIR:** `ALairGameMode` owns BoardSystem, TurnManager, RulesEngine, etc.

---

## Development Terms

### Phase 1
The foundation build. Focus: Board, units, purchase, turn management. No combat or mining.

**Timeline:** 2-3 weeks

### Phase 2
Adding gameplay systems: Mining and Combat.

**Status:** Planned, not yet implemented.

### Phase 3
Adding complexity: Outposts, gates, elite units, special abilities.

**Status:** Planned, not yet implemented.

### Stable API
Public C++ functions whose signatures are frozen and cannot change without versioning.

**Why it matters:** Phase 2 can add new functions but cannot modify Phase 1 stable functions. This prevents breaking existing code.

**Example Stable Functions:**
- `ATile::CanPlaceUnit()`
- `UBoardSystemComponent::GetTileAt()`
- `URulesEngineComponent::CanAffordUnit()`

### Stable API Contract
The agreement that Phase 2+ will extend, not modify, Phase 1's stable functions.

**If you must change:** Create `FunctionName_V2()`, mark old as DEPRECATED.

### Phase Transition Contract
The agreement defining what Phase 2 can assume about Phase 1 and what it must NOT break.

**What Phase 2 can assume:**
- BoardSystem works and is battle-ready
- RulesEngine validation exists
- Sub-slot logic is complete

**What Phase 2 must NOT do:**
- Change stable API signatures
- Break Phase 1 acceptance tests

### Acceptance Test
A specific, measurable test that must pass for a phase to be considered complete.

**Format:** Given/When/Then

**Example:** 
- **Given:** Player has 200 gold
- **When:** Player purchases Footman (costs 50)
- **Then:** Gold becomes 150, Footman spawns at base

**Phase 1:** 10 acceptance tests must pass.

### Spec Lock
A document versioning policy where changes require a version bump (e.g., v1.5 → v1.6).

**Purpose:** Prevents silent edits to production specifications.

**LAIR Status:** Implementation Plan v1.5 is LOCKED.

---

## Phase-Specific Terms

### Test Board
The simplified 10x10 board used for Phase 1 testing.

**vs Rulebook Board:** The full 20x20 board with dragon lair, gates, outposts (Phase 3+)

### Mining Tile
A tile where miners can draw mining cards to discover resources (Phase 2+).

**Status in Phase 1:** Not implemented.

### Mystery Tile
A hidden tile that reveals content when entered (treasure, trap, or monster) (Phase 2+).

**Status in Phase 1:** Not implemented.

### Outpost
A capturable location where units can spawn (Phase 3+).

**Status in Phase 1:** Not implemented.

### Dragon Lair
The central 4-tile chamber where the dragon boss lives (Phase 3+).

**Status in Phase 1:** Not implemented.

### Treasure
Items found in mines or mystery tiles that provide bonuses (gold, units, special items) (Phase 2+).

**Status in Phase 1:** Not implemented.

---

## Quick Reference Tables

### Unit Stats (Phase 1)

| Unit | Cost | Movement | HP | Sub-Slot Size |
|------|------|----------|----|----|
| Miner | 40gp | 4 | 1 | 1 |
| Footman | 50gp | 5 | 1 | 1 |
| Wagon | 70gp | 6 | 1 | 2 |

### Movement Costs

| Move Type | Cost |
|-----------|------|
| Orthogonal (N/S/E/W) | 1 point |
| Diagonal (NE/NW/SE/SW) | 2 points |

### Tile Capacity

| Scenario | Can Place? |
|----------|------------|
| Empty tile + 1 footman (size 1) | ✅ Yes (3 slots free) |
| Tile with 3 footmen + 1 wagon (size 2) | ❌ No (1 slot free, need 2) |
| Tile with 2 wagons (4 slots used) | ❌ No (0 slots free) |
| Tile with 4 footmen | ❌ No (0 slots free) |

---

## Common Confusions Clarified

### "Can I move diagonally if an enemy is adjacent?"
**Yes!** LAIR does not have corner blocking. Diagonal movement always costs 2 points regardless of adjacent units.

### "How many units fit on one tile?"
**It depends on sub-slot size:**
- 4 normal units (4 × 1 = 4 sub-slots)
- 2 wagons (2 × 2 = 4 sub-slots)
- 2 normal units + 1 wagon (2×1 + 1×2 = 4 sub-slots)

### "What's the difference between a Tile and a Sub-Slot?"
- **Tile** = One square on the board
- **Sub-Slot** = Quarter of a tile (for unit placement)
- Think of a tile like a parking lot, sub-slots are the parking spaces

### "What does 'Stable API' mean for non-programmers?"
It means once Phase 1 is done, we promise not to change how core functions work. Phase 2 adds new features but doesn't break Phase 1. This keeps the codebase stable.

### "Why is the Wagon size 2?"
Wagons are larger units (physically bigger on the board) to represent carts carrying minerals. They need more space than a single soldier.

---

## Abbreviations

| Abbr | Full Term |
|------|-----------|
| PIE | Play In Editor |
| HP | Hit Points |
| AT | Attack (dice values) |
| gp | Gold Pieces |
| DT | Data Table |
| BP | Blueprint |
| UE5 | Unreal Engine 5 |
| AI | Artificial Intelligence |
| API | Application Programming Interface |
| NPC | Non-Player Character |
| UI | User Interface |

---

## Related Documents

- **Implementation_Plan_Phase1_v1.5.md** - Full technical specification and build instructions
- **LAIR_UE5_Technical_Specification.md** - Architecture reference
- **LAIR_Quick_Start_Guide.md** - Setup and troubleshooting
- **Master_Build_Prompt_v3.0.md** - Claude Code orchestration instructions

---

**Version:** 1.0  
**Last Updated:** January 21, 2026  
**Status:** Reference Document  
**Audience:** All contributors (technical and non-technical)

**For questions or clarifications, see the full Implementation Plan or ask the development team.**
