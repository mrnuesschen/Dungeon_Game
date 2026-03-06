#pragma once

#include <memory>
#include <string>
#include <vector>

#include "dungeon/DungeonGenerator.h"
#include "dungeon/DungeonTraversal.h"
#include "ui/Button.h"

enum class DungeonSceneAction {
    None,
    EncounterTriggered,
    ReachedExit,
    ReturnToWorld
};

class DungeonScene {
public:
    DungeonScene();

    void EnterNewDungeon(const std::string& playerName);
    void AdvanceToNextFloor();
    int GetCurrentFloor() const;
    int GetMinEncounterEnemies() const;
    int GetMaxEncounterEnemies() const;
    int GetEncounterEnemyLevelBonus() const;
    DungeonSceneAction Update();
    void Draw() const;

    int GetTriggeredEncounterTile() const;
    void ClearEncounter(int tileIndex);

private:
    void GenerateCurrentFloor();
    bool IsEncounterTile(int x, int y) const;
    int GetTileIndex(int x, int y) const;
    void PlaceRandomEncounters(int count);

    bool initialized;
    int currentFloor;
    std::string playerName;
    dungeon::DungeonMap map;
    std::unique_ptr<dungeon::DungeonTraversal> traversal;
    std::vector<bool> activeEncounters;
    int triggeredEncounterTile;

    ui::Button leaveDungeonButton;
};
