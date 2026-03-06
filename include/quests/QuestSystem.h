#pragma once

#include <vector>

#include "GameTypes.h"
#include "items/ItemSystem.h"

namespace quests {

enum class QuestId {
    GraveyardSweep,
    SlimeContainment,
    RelicRetrieval,
    BrokenCaravan,
    FloodedCatacombs,
    NightWatch,
};

enum class QuestObjectiveType {
    DefeatEnemyFamily,
    RetrieveItemOnFloor,
};

enum class QuestEnemyFamily {
    Any,
    Slime,
    Skeleton,
};

struct QuestObjective {
    QuestObjectiveType type;
    int requiredCount;
    int minFloor;
    QuestEnemyFamily enemyFamily;
    items::ItemId requiredItem;
    int targetFloor;
};

struct QuestDefinition {
    QuestId id;
    const char* title;
    const char* target;
    const char* description;
    const char* reward;
    QuestObjective objective;
};

struct QuestItemSpawn {
    QuestId questId;
    items::ItemId itemId;
    int floor;
};

const std::vector<QuestDefinition>& GetAllQuestDefinitions();
std::vector<QuestDefinition> GetRandomGuildQuestSubset(int maxCount);
const QuestDefinition& GetQuestDefinition(QuestId id);

class QuestLog {
public:
    QuestLog();

    bool AcceptQuest(QuestId id);
    bool IsAccepted(QuestId id) const;
    bool IsCompleted(QuestId id) const;
    int GetProgress(QuestId id) const;
    int GetRequiredCount(QuestId id) const;

    void RegisterBattleVictory(int floor, const std::vector<EnemyArchetype>& defeatedEnemies);
    void RegisterQuestItemCollected(QuestId questId, items::ItemId itemId, int floor);

    std::vector<QuestItemSpawn> GetPendingQuestItemSpawnsForFloor(int floor) const;

private:
    struct QuestState {
        bool accepted = false;
        bool completed = false;
        int progress = 0;
    };

    std::vector<QuestState> states;

    static size_t QuestIndex(QuestId id);
};

} // namespace quests
