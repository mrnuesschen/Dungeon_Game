#pragma once

#include <optional>
#include <memory>
#include <string>
#include <vector>

#include "dungeon/DungeonGenerator.h"
#include "dungeon/DungeonTraversal.h"
#include "quests/QuestSystem.h"
#include "ui/Button.h"

enum class DungeonSceneAction {
    None,
    EncounterTriggered,
    QuestItemCollected,
    ReachedExit,
    ReturnToWorld
};

struct DungeonQuestItemCollection {
    quests::QuestId questId;
    items::ItemId itemId;
    int floor;
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
    void SetQuestItemSpawnsForCurrentFloor(const std::vector<quests::QuestItemSpawn>& spawns);
    std::optional<DungeonQuestItemCollection> ConsumeCollectedQuestItem();

private:
    void GenerateCurrentFloor();
    bool IsEncounterTile(int x, int y) const;
    int GetTileIndex(int x, int y) const;
    void PlaceRandomEncounters(int count);
    void PlaceQuestItemsForCurrentFloor();
    bool IsQuestItemTile(int x, int y) const;

    bool initialized;
    int currentFloor;
    std::string playerName;
    dungeon::DungeonMap map;
    std::unique_ptr<dungeon::DungeonTraversal> traversal;
    std::vector<bool> activeEncounters;
    std::vector<std::optional<quests::QuestItemSpawn>> questItemByTile;
    std::vector<quests::QuestItemSpawn> requestedQuestSpawns;
    int triggeredEncounterTile;
    std::optional<DungeonQuestItemCollection> collectedQuestItem;
    int heldMoveDx;
    int heldMoveDy;
    float heldMoveTimer;

    ui::Button leaveDungeonButton;
};
