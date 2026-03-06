# Dungeon Game (C++ Windowed)

A simple C++ game project using `raylib` as a starting point for a visual, turn-based battle game.

## Project Structure (Modular)

- `src/main.cpp`
	- App loop and scene switching (`MainMenu`, `CharacterCreation`, `WorldMap`, `CityMap`, `Dungeon`, `Battle`, `Gallery`)
- `include/scenes/MainMenuScene.h` + `src/scenes/MainMenuScene.cpp`
	- Main menu with buttons: `New Game`, `Load Game`, `Gallery`, `Quit`
- `include/scenes/CharacterCreationScene.h` + `src/scenes/CharacterCreationScene.cpp`
	- Character setup: class, avatar, and player name
- `include/scenes/BattleScene.h` + `src/scenes/BattleScene.cpp`
	- Turn-based combat and battle UI
- `include/scenes/WorldMapScene.h` + `src/scenes/WorldMapScene.cpp`
	- World map with multiple locations (Dungeon and City)
- `include/scenes/CityMapScene.h` + `src/scenes/CityMapScene.cpp`
	- City sub-map with Tavern, Guild, Shop, and Inn (shop purchases with gold)
- `include/scenes/DungeonScene.h` + `src/scenes/DungeonScene.cpp`
	- Walkable dungeon map (W/A/S/D), encounter triggers, and dungeon exit handling
- `include/scenes/GalleryScene.h` + `src/scenes/GalleryScene.cpp`
	- Gallery view with unlockable image slots
- `include/ui/Button.h` + `src/ui/Button.cpp`
	- Shared button module (input + rendering)
- `include/SaveSystem.h` + `src/SaveSystem.cpp`
	- Save/load for game state and gallery progress
- `include/GameTypes.h`
	- Shared data types and enums used across scenes/modules
- `include/entities/Player.h` + `src/entities/Player.cpp`
	- Player class with 6 presets: Tank, Knight, Assassin, Mage, Healer, Alchemist
- `include/entities/Enemy.h` + `src/entities/Enemy.cpp`
	- Base enemy class
- `include/entities/Slime.h` + `src/entities/Slime.cpp`
	- Slime variants: Small, Medium, Large
- `include/entities/Skeleton.h` + `src/entities/Skeleton.cpp`
	- Skeleton variants: Lesser Skeleton, Skeleton, Skeleton Warrior
- `include/entities/EnemyFactory.h` + `src/entities/EnemyFactory.cpp`
	- Factory for creating concrete enemy types
- `include/dungeon/DungeonGenerator.h` + `src/dungeon/DungeonGenerator.cpp`
	- Random maze/dungeon generation as a matrix (`TileType` grid)
- `include/dungeon/DungeonTraversal.h` + `src/dungeon/DungeonTraversal.cpp`
	- Grid-based movement logic with `W/A/S/D` (one tile per key press)
- `include/items/ItemSystem.h` + `src/items/ItemSystem.cpp`
	- Item definitions and encounter loot generation (equipment, consumables, quest items), including shop prices
- `include/items/Inventory.h` + `src/items/Inventory.cpp`
	- Player inventory container and stack management

## Economy

- Gold is dropped by defeated monster groups in battles
- Gold can be spent in the city shop to buy multiple items

## Main Menu Features

- `New Game`: opens character creation, then world map
- `Dungeon` on world map: opens a generated maze map and allows tile-based navigation
- `Load Game`: loads an existing save (`savegame.txt`)
- `Gallery`: opens unlocked gallery images
- `Quit`: exits the game

## Character Creation

- Class selection happens only when starting a new game
- Avatar selection from `assets/sprites/player/`
- Free text player name input

## Asset Locations

- Unlockable gallery images: `assets/gallery/`
- Player sprites: `assets/sprites/player/`
- Enemy sprites: `assets/sprites/enemies/`
- UI graphics (icons, panels, buttons): `assets/ui/`

Recommended formats: `.png` (with transparency), optional `.jpg` for large background images.

Enemy sprite filenames used by the enemy classes:

- `assets/sprites/enemies/slime_level_0.png`
- `assets/sprites/enemies/slime_level_20.png`
- `assets/sprites/enemies/slime_level_40.png`
- `assets/sprites/enemies/skeleton_level_0.png`
- `assets/sprites/enemies/skeleton_level_20.png`
- `assets/sprites/enemies/skeleton_level_40.png`

## Requirements

- CMake (>= 3.16)
- A C++17-compatible compiler (MSVC, MinGW, clang, ...)
- Internet access on first build (CMake fetches `raylib` automatically)

## Build (Windows)

```powershell
cmake -S . -B build
cmake --build build --config Release
```

The executable is generated here depending on your generator:

- Multi-config (Visual Studio): `build/Release/DungeonGame.exe`
- Single-config (Ninja/MinGW): `build/DungeonGame.exe`

## Run

The game opens in its own window (no terminal gameplay).

Visual Studio generator:

```powershell
./build/Release/DungeonGame.exe
```

Ninja/MinGW:

```powershell
./build/DungeonGame.exe
```
