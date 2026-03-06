#include "scenes/DungeonScene.h"

#include <algorithm>
#include <random>

#include "raylib.h"

namespace {

struct FloorEncounterDifficulty {
    int minEnemies;
    int maxEnemies;
    int enemyLevelBonus;
};

FloorEncounterDifficulty GetDifficultyForFloor(int floor) {
    if (floor <= 2) {
        return FloorEncounterDifficulty{1, 2, floor / 2};
    }

    if (floor <= 5) {
        return FloorEncounterDifficulty{3, 3, 1 + floor / 2};
    }

    return FloorEncounterDifficulty{3, 8, 2 + floor / 2};
}

}

DungeonScene::DungeonScene()
    : initialized(false),
    currentFloor(0),
      playerName("Hero"),
      map(),
      traversal(nullptr),
      activeEncounters(),
      triggeredEncounterTile(-1),
      leaveDungeonButton{Rectangle{740.0f, 468.0f, 180.0f, 52.0f}, "World Map", true} {}

void DungeonScene::EnterNewDungeon(const std::string& value) {
    playerName = value.empty() ? "Hero" : value;
    currentFloor = 0;
    GenerateCurrentFloor();
    initialized = true;
}

void DungeonScene::AdvanceToNextFloor() {
    if (!initialized) {
        return;
    }

    currentFloor++;
    GenerateCurrentFloor();
}

int DungeonScene::GetCurrentFloor() const {
    return currentFloor;
}

int DungeonScene::GetMinEncounterEnemies() const {
    return GetDifficultyForFloor(currentFloor).minEnemies;
}

int DungeonScene::GetMaxEncounterEnemies() const {
    return GetDifficultyForFloor(currentFloor).maxEnemies;
}

int DungeonScene::GetEncounterEnemyLevelBonus() const {
    return GetDifficultyForFloor(currentFloor).enemyLevelBonus;
}

DungeonSceneAction DungeonScene::Update() {
    const float sx = static_cast<float>(GetScreenWidth()) / 960.0f;
    const float sy = static_cast<float>(GetScreenHeight()) / 540.0f;
    leaveDungeonButton.bounds = Rectangle{740.0f * sx, 468.0f * sy, 180.0f * sx, 52.0f * sy};

    if (!initialized) {
        return DungeonSceneAction::None;
    }

    if (ui::IsPressed(leaveDungeonButton) || IsKeyPressed(KEY_ESCAPE)) {
        return DungeonSceneAction::ReturnToWorld;
    }

    const dungeon::MoveResult moveResult = traversal->UpdateFromInputWASD();
    if (moveResult == dungeon::MoveResult::NoInput || moveResult == dungeon::MoveResult::Blocked) {
        return DungeonSceneAction::None;
    }

    const dungeon::CellPos& pos = traversal->GetPlayerPosition();
    const int tileIndex = GetTileIndex(pos.x, pos.y);
    if (tileIndex >= 0 && tileIndex < static_cast<int>(activeEncounters.size()) && activeEncounters[static_cast<size_t>(tileIndex)]) {
        triggeredEncounterTile = tileIndex;
        return DungeonSceneAction::EncounterTriggered;
    }

    if (moveResult == dungeon::MoveResult::ReachedExit) {
        return DungeonSceneAction::ReachedExit;
    }

    return DungeonSceneAction::None;
}

void DungeonScene::Draw() const {
    const float sx = static_cast<float>(GetScreenWidth()) / 960.0f;
    const float sy = static_cast<float>(GetScreenHeight()) / 540.0f;
    const float sf = std::min(sx, sy);

    DrawText("Dungeon", static_cast<int>(24.0f * sx), static_cast<int>(18.0f * sy), static_cast<int>(44.0f * sf), RAYWHITE);
    DrawText(TextFormat("Explorer: %s", playerName.c_str()), static_cast<int>(26.0f * sx), static_cast<int>(64.0f * sy), static_cast<int>(20.0f * sf), LIGHTGRAY);
    DrawText(TextFormat("Floor: %d", currentFloor), static_cast<int>(320.0f * sx), static_cast<int>(64.0f * sy), static_cast<int>(20.0f * sf), Color{220, 200, 140, 255});
    DrawText(TextFormat("Enemy Pack Size: %d-%d", GetMinEncounterEnemies(), GetMaxEncounterEnemies()), static_cast<int>(470.0f * sx), static_cast<int>(64.0f * sy), static_cast<int>(20.0f * sf), Color{210, 180, 140, 255});

    if (!initialized || traversal == nullptr) {
        DrawText("Dungeon not initialized.", static_cast<int>(26.0f * sx), static_cast<int>(110.0f * sy), static_cast<int>(24.0f * sf), GRAY);
        return;
    }

    const float mapTop = 96.0f * sy;
    const float mapBottom = 450.0f * sy;
    const float mapHeight = mapBottom - mapTop;
    const float mapWidth = static_cast<float>(GetScreenWidth()) - 32.0f * sx;
    const float tileSize = std::min(mapWidth / static_cast<float>(std::max(1, map.width)), mapHeight / static_cast<float>(std::max(1, map.height)));
    const float drawWidth = tileSize * static_cast<float>(map.width);
    const float drawHeight = tileSize * static_cast<float>(map.height);
    const float startX = (static_cast<float>(GetScreenWidth()) - drawWidth) * 0.5f;
    const float startY = mapTop + (mapHeight - drawHeight) * 0.5f;

    const dungeon::CellPos playerPos = traversal->GetPlayerPosition();

    for (int y = 0; y < map.height; ++y) {
        for (int x = 0; x < map.width; ++x) {
            const float px = startX + static_cast<float>(x) * tileSize;
            const float py = startY + static_cast<float>(y) * tileSize;
            const Rectangle tileRect{px, py, tileSize, tileSize};

            const dungeon::TileType tile = map.tiles[static_cast<size_t>(y)][static_cast<size_t>(x)];
            Color color = Color{30, 35, 45, 255};
            if (tile == dungeon::TileType::Wall) {
                color = Color{17, 20, 28, 255};
            } else if (tile == dungeon::TileType::Floor) {
                color = Color{80, 86, 102, 255};
            } else if (tile == dungeon::TileType::Start) {
                color = Color{76, 130, 88, 255};
            } else if (tile == dungeon::TileType::Exit) {
                color = Color{156, 118, 76, 255};
            }

            DrawRectangleRec(tileRect, color);

            if (IsEncounterTile(x, y)) {
                DrawCircle(static_cast<int>(px + tileSize * 0.5f), static_cast<int>(py + tileSize * 0.5f), tileSize * 0.2f, Color{210, 80, 80, 255});
            }
        }
    }

    DrawCircle(static_cast<int>(startX + (static_cast<float>(playerPos.x) + 0.5f) * tileSize),
               static_cast<int>(startY + (static_cast<float>(playerPos.y) + 0.5f) * tileSize),
               tileSize * 0.35f,
               Color{78, 210, 255, 255});

    ui::DrawButton(leaveDungeonButton);
    DrawText("W/A/S/D: move one tile | Red dot: encounter | Brown tile: exit", static_cast<int>(24.0f * sx), static_cast<int>(502.0f * sy), static_cast<int>(18.0f * sf), Color{195, 205, 220, 255});
}

int DungeonScene::GetTriggeredEncounterTile() const {
    return triggeredEncounterTile;
}

void DungeonScene::ClearEncounter(int tileIndex) {
    if (tileIndex >= 0 && tileIndex < static_cast<int>(activeEncounters.size())) {
        activeEncounters[static_cast<size_t>(tileIndex)] = false;
    }
    triggeredEncounterTile = -1;
}

void DungeonScene::GenerateCurrentFloor() {
    map = dungeon::DungeonGenerator::GenerateRandomMaze(31, 21);
    traversal = std::make_unique<dungeon::DungeonTraversal>(map);
    activeEncounters.assign(static_cast<size_t>(map.width * map.height), false);
    PlaceRandomEncounters(8);
    triggeredEncounterTile = -1;
}

bool DungeonScene::IsEncounterTile(int x, int y) const {
    const int index = GetTileIndex(x, y);
    if (index < 0 || index >= static_cast<int>(activeEncounters.size())) {
        return false;
    }
    return activeEncounters[static_cast<size_t>(index)];
}

int DungeonScene::GetTileIndex(int x, int y) const {
    if (x < 0 || y < 0 || x >= map.width || y >= map.height) {
        return -1;
    }
    return y * map.width + x;
}

void DungeonScene::PlaceRandomEncounters(int count) {
    std::vector<int> floorTiles;
    floorTiles.reserve(static_cast<size_t>(map.width * map.height));

    for (int y = 0; y < map.height; ++y) {
        for (int x = 0; x < map.width; ++x) {
            const dungeon::TileType tile = map.tiles[static_cast<size_t>(y)][static_cast<size_t>(x)];
            if (tile == dungeon::TileType::Wall || tile == dungeon::TileType::Start || tile == dungeon::TileType::Exit) {
                continue;
            }
            floorTiles.push_back(GetTileIndex(x, y));
        }
    }

    std::mt19937 rng(std::random_device{}());
    std::shuffle(floorTiles.begin(), floorTiles.end(), rng);

    const int encounterCount = std::min(count, static_cast<int>(floorTiles.size()));
    for (int i = 0; i < encounterCount; ++i) {
        activeEncounters[static_cast<size_t>(floorTiles[static_cast<size_t>(i)])] = true;
    }
}
