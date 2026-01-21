# LAIR - Quick Start Guide: GitHub to Playable Game
**High-Level Deployment Steps for Windows**

---

## Document Metadata

**Document Type:** Getting Started & Troubleshooting Guide  
**Version:** 1.1 (Post-Audit Update)  
**Last Updated:** January 21, 2026  
**Compatible Engine:** Unreal Engine 5.3.2  
**Status:** Final - Onboarding Guide  
**Document Owner:** Development Team  

---

## Document Hierarchy

**THIS IS THE GETTING STARTED GUIDE**

| Document | Purpose | Authority Level |
|----------|---------|-----------------|
| Implementation_Plan_Phase1_v1.5.md | Phase 1 build instructions, file order, acceptance tests | **PRIMARY for Phase 1** |
| LAIR_UE5_Technical_Specification.md | Architecture reference, data structures, full system design | Reference |
| **LAIR_Quick_Start_Guide.md (this doc)** | **Getting started, onboarding, troubleshooting** | **GETTING STARTED** |
| LAIR_Rules_Glossary_v1.0.md | Game terminology, rule clarifications | Developer Reference |
| Master_Build_Prompt_v3.0.md | Claude Code orchestration instructions | Build Orchestration |

**Purpose of This Document:**
- Install required software
- Clone the repository
- Open project in Unreal Engine
- Basic testing and troubleshooting
- Package for standalone play

**For Detailed Implementation:** See Implementation_Plan_Phase1_v1.5.md for:
- Data table creation instructions
- File generation order
- Phase 1 scope and acceptance tests
- Rules clarifications

---

## Overview: The Journey

```
GitHub Repository → Your Computer → Unreal Editor → Playable .exe
     (code)         (clone/setup)    (compile/test)    (package)
```

**Time Required:** 2-4 hours (mostly download/install time)  
**Disk Space Needed:** ~100GB (UE5 is large!)

---

## Phase 1: One-Time Setup (Install Required Software)

### What You Need

| Software | Why | Download Link | Size |
|----------|-----|---------------|------|
| **Visual Studio 2022** | Compiles C++ code | https://visualstudio.microsoft.com/downloads/ | ~20GB |
| **Unreal Engine 5.3+** | The game engine | https://www.unrealengine.com/download | ~50GB |
| **Git** | Downloads code from GitHub | https://git-scm.com/download/win | 50MB |
| **Git LFS** | Handles large asset files | https://git-lfs.github.com/ | 5MB |

### Installation Steps

#### 1. Install Visual Studio 2022
1. Download the **Community Edition** (free)
2. When installing, check these workloads:
   - ✅ **Desktop development with C++**
   - ✅ **Game development with C++**
3. Install and restart computer

#### 2. Install Unreal Engine 5.3
1. Download **Epic Games Launcher**
2. Create free Epic account
3. Go to **Unreal Engine** tab → **Install Engine** → Choose version **5.3.2**
4. Let it install (takes 1-2 hours)

#### 3. Install Git + Git LFS
1. Install Git with default settings
2. Install Git LFS
3. Open Command Prompt and run: `git lfs install`

---

## Phase 2: Get the Code from GitHub

### Clone the Repository

**Option A: Command Line**
```cmd
cd C:\Projects
git clone https://github.com/YourUsername/Lair.git
cd Lair
```

**Option B: GitHub Desktop (Easier)**
1. Install GitHub Desktop: https://desktop.github.com/
2. Click **Clone a repository**
3. Enter URL and choose folder
4. Click **Clone**

### Verify You Have These Files
```
Lair/
├── Config/
├── Content/
├── Source/
│   └── Lair/
├── Lair.uproject  ← This is the key file!
└── README.md
```

---

## Phase 3: Open in Unreal Engine

### Generate Project Files

1. Navigate to your Lair folder
2. **Right-click on `Lair.uproject`**
3. Select **"Generate Visual Studio project files"**
4. Wait 30-60 seconds
5. You should now see `Lair.sln` (Visual Studio solution file)

### Open the Project

**Double-click `Lair.uproject`**

What happens:
1. Unreal Engine 5 opens
2. It detects C++ code
3. Popup: "Would you like to rebuild Lair now?" → Click **Yes**
4. Visual Studio compiles in background (5-15 minutes)
5. If successful, Unreal Editor fully opens

**If compilation fails:**
- Close editor
- Delete `Binaries/` and `Intermediate/` folders
- Right-click `Lair.uproject` → Generate VS project files again
- Try opening again

---

## Phase 4: Set Up Data Tables (One-Time)

Your C++ code has struct definitions, but you need to create the actual data assets.

### Quick Overview

For Phase 1, you need 4 data tables:
1. **DT_Units** - Unit stats (Miner, Wagon, Footman)
2. **DT_TileTypes** - Tile properties (Empty, PlayerBase)
3. **DT_Factions** - Player colors (Player1, Player2)
4. **DT_BoardLayout_Test10x10** - 10x10 test board layout

### Creating a Data Table

1. In **Content Browser**, navigate to `Content/Data/`
2. Right-click → **Miscellaneous** → **Data Table**
3. Choose row structure (e.g., `FUnitData` for units)
4. Name it (e.g., `DT_Units`)
5. Double-click to open
6. Click **Add Row** and fill in values

**For Detailed Data:** See Implementation_Plan_Phase1_v1.5.md, section "Data Tables (Phase 1 Subset)" for:
- Exact values for all 3 units
- Tile type definitions
- Board layout structure
- Sample data for each table

### Link Data Tables to Game Mode

1. Open `Content/Blueprints/GameMode/BP_LairGameMode`
2. In **Details** panel, find **Data** section
3. Assign each data table variable:
   - Units Data Table → `DT_Units`
   - Tile Types Data Table → `DT_TileTypes`
   - Factions Data Table → `DT_Factions`
   - Board Layout → `DT_BoardLayout_Test10x10`
4. **Compile** and **Save**

---

## Phase 5: Test in Editor

### Play the Game

1. Open main level: `Content/Maps/GameBoard.umap`
2. Click **Play** button (▶️) at top
3. Game runs inside editor window

### What to Check
- ✅ Board spawns correctly
- ✅ Can purchase units
- ✅ Units appear on board
- ✅ Mining works
- ✅ Combat resolves
- ✅ No red errors in **Output Log**

**Debug Tips:**
- Press **`** (tilde) to open console
- Type `stat fps` to see performance
- Use **Blueprint Debugger** to step through logic

---

## Phase 6: Package for Standalone Play

### Configure Project

1. **Edit** → **Project Settings**
2. **Project** → **Maps & Modes**
   - Set **Game Default Map** to `/Game/Maps/GameBoard`
3. **Project** → **Packaging**
   - Build Configuration: **Development** (for testing) or **Shipping** (for release)
   - Check **Use Pak File**

### Build the Game

1. **Platforms** → **Windows** → **Package Project**
2. Choose output folder (e.g., `C:\Projects\Lair\Builds\Windows`)
3. Click **Select Folder**
4. Wait 10-30 minutes (grab coffee ☕)

### Output Structure
```
Builds/Windows/WindowsNoEditor/
├── Lair.exe          ← This is your game!
├── Lair/
│   └── Content/
│       └── Paks/     ← Compressed game assets
└── Engine/           ← Required UE5 runtime files
```

### Run Your Game

1. Navigate to `WindowsNoEditor/` folder
2. **Double-click `Lair.exe`**
3. Game launches in fullscreen!

---

## Phase 7: Distribute to Others

### Sharing the Game

**To share with others:**
1. Zip the entire `WindowsNoEditor` folder
2. Upload to Google Drive / Dropbox / itch.io
3. Others download, extract, and run `Lair.exe`

**Expected Size:** 2-5GB (depending on assets)

### Publishing Options

| Platform | Difficulty | Notes |
|----------|------------|-------|
| **Itch.io** | Easy | Free hosting, indie-friendly |
| **Steam** | Medium | $100 fee, requires Steamworks integration |
| **Epic Games Store** | Hard | Must apply, high quality bar |
| **Direct Download** | Easy | Host on your own website |

---

## Common Issues & Quick Fixes

### "Project won't open"
**Fix:** Delete `Binaries/`, `Intermediate/`, `Saved/` folders, then right-click .uproject → Generate VS project files

### "Build failed"
**Fix:** Open `Lair.sln` in Visual Studio → Build → Clean Solution → Rebuild Solution

### "Data is missing/empty"
**Fix:** Verify data tables are created and assigned to BP_LairGameMode

### "Packaged game won't start"
**Fix:** Repackage with **Full Rebuild** option checked

### "Git LFS files didn't download"
**Fix:** Open Command Prompt in project folder, run `git lfs pull`

---

## The Complete Workflow (At a Glance)

```
┌─────────────────────────────────────────────────────────────┐
│ 1. SETUP (One Time)                                         │
│    - Install VS 2022, UE5, Git, Git LFS                     │
└─────────────────┬───────────────────────────────────────────┘
                  │
┌─────────────────▼───────────────────────────────────────────┐
│ 2. CLONE                                                     │
│    - git clone https://github.com/YourUsername/Lair.git     │
└─────────────────┬───────────────────────────────────────────┘
                  │
┌─────────────────▼───────────────────────────────────────────┐
│ 3. GENERATE                                                  │
│    - Right-click Lair.uproject → Generate VS project files  │
└─────────────────┬───────────────────────────────────────────┘
                  │
┌─────────────────▼───────────────────────────────────────────┐
│ 4. COMPILE                                                   │
│    - Double-click Lair.uproject → Click "Yes" to rebuild    │
└─────────────────┬───────────────────────────────────────────┘
                  │
┌─────────────────▼───────────────────────────────────────────┐
│ 5. SETUP DATA                                                │
│    - Create 9 data tables in Content/Data/                  │
│    - Assign to BP_LairGameMode                              │
└─────────────────┬───────────────────────────────────────────┘
                  │
┌─────────────────▼───────────────────────────────────────────┐
│ 6. TEST                                                      │
│    - Open GameBoard.umap → Click Play → Test gameplay       │
└─────────────────┬───────────────────────────────────────────┘
                  │
┌─────────────────▼───────────────────────────────────────────┐
│ 7. PACKAGE                                                   │
│    - Platforms → Windows → Package Project                  │
│    - Wait 10-30 minutes                                      │
└─────────────────┬───────────────────────────────────────────┘
                  │
┌─────────────────▼───────────────────────────────────────────┐
│ 8. PLAY                                                      │
│    - Navigate to WindowsNoEditor/ → Run Lair.exe            │
└──────────────────────────────────────────────────────────────┘
```

---

## Iterative Development Workflow

After initial setup, your day-to-day workflow is:

### Making Code Changes
1. Claude Code generates new C++ code
2. Commit to GitHub
3. Pull changes: `git pull`
4. In Unreal Editor, click **Compile** button
5. Test changes with **Play**

### Making Data Changes
1. Open data table (e.g., DT_Units)
2. Edit values (e.g., change Footman cost from 50 to 60)
3. Save
4. Test immediately with **Play** (no compile needed!)

### Making Blueprint Changes
1. Open blueprint (e.g., WBP_MainHUD)
2. Drag nodes, change properties
3. **Compile** and **Save** blueprint
4. Test with **Play**

### Packaging New Builds
Only package when you want to test outside the editor or share with others:
1. Make sure everything works in editor
2. **Platforms** → **Windows** → **Package Project**
3. Test the .exe before sharing

---

## Key Concepts to Understand

### What is Compilation?
- **C++ Code** → Visual Studio compiles → **Binary (.dll files)**
- Unreal Editor uses these binaries to run your game
- Happens automatically when opening project or clicking Compile button

### What is Cooking?
- **Assets** (meshes, textures, blueprints) → Unreal "cooks" → **Platform-optimized files**
- Only happens during packaging
- Converts editor-friendly formats to runtime-efficient formats

### What is Packaging?
- **Full Process:** Compile → Cook → Bundle → Create .exe
- Creates standalone game that doesn't need Unreal Editor
- Takes longer because it processes ALL content

### Editor vs. Packaged Game
| Aspect | Editor | Packaged Game |
|--------|--------|---------------|
| Speed | Fast iteration | Slower to rebuild |
| Performance | Lower FPS | Higher FPS |
| Debugging | Full tools | Limited |
| File Size | N/A | 2-5GB |
| Use Case | Development | Testing/Release |

---

## Recommended Development Flow

### Week 1: Core Systems
- Clone repo
- Get project opening successfully
- Create data tables
- Test basic purchase → placement flow

### Week 2-3: Gameplay
- Implement mining system
- Test combat
- Add UI widgets
- Play full games in editor

### Week 4: Polish
- Balance data tables
- Fix bugs
- Add visual/audio polish
- Package first standalone build

### Week 5: Testing
- Playtest packaged builds
- Iterate on balance
- Package final release

---

## Tips for Success

### 1. Commit Often to GitHub
```bash
git add .
git commit -m "Implemented mining system"
git push
```
- Save your work frequently
- Makes it easy to rollback if something breaks

### 2. Use Small Test Maps
- Create a tiny 5x5 board for quick testing
- Don't test on the full 20x20 board every time
- Saves iteration time

### 3. Keep Output Log Visible
- **Window** → **Developer Tools** → **Output Log**
- Check for errors after every change
- Red text = something is wrong

### 4. Test in Editor First
- Only package when you need to test performance or share
- 95% of development happens in editor with Play button

### 5. Keep Backups
- Before major changes, zip your project folder
- Or create a Git branch: `git checkout -b experimental-feature`

---

## Summary: The Essentials

**To get from GitHub to playable game:**

1. **Install:** VS 2022, UE5, Git, Git LFS
2. **Clone:** `git clone [repo]`
3. **Generate:** Right-click .uproject → Generate VS project files
4. **Open:** Double-click .uproject → Let it compile
5. **Setup:** Create data tables and assign to game mode
6. **Test:** Click Play in editor
7. **Package:** Platforms → Windows → Package Project
8. **Play:** Run Lair.exe

**That's it!** The complexity is in steps 5-6 (setting up gameplay), but the infrastructure is straightforward.

---

## Next Steps

Now that you understand the deployment process, you can:

1. **Start with Phase 1 Implementation** (from the technical spec)
   - Get basic board and units working
   - Test in editor frequently

2. **Iterate Incrementally**
   - Add one feature at a time
   - Test after each addition
   - Package weekly to test performance

3. **Share Early Builds**
   - Get feedback from playtesters
   - Iterate on balance and UX

4. **Plan for Online Multiplayer** (Phase 5)
   - Once local hotseat works perfectly
   - Add networking layer
   - Requires server infrastructure

Good luck building Lair! 🐉
