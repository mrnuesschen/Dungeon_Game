#include "quests/QuestSystem.h"

#include <algorithm>
#include <numeric>

#include "raylib.h"

namespace quests {

namespace {

QuestEnemyFamily GetEnemyFamily(EnemyArchetype type) {
    switch (type) {
    case EnemyArchetype::SmallSlime:
    case EnemyArchetype::MediumSlime:
    case EnemyArchetype::LargeSlime:
        return QuestEnemyFamily::Slime;
    case EnemyArchetype::LesserSkeleton:
    case EnemyArchetype::Skeleton:
    case EnemyArchetype::SkeletonWarrior:
        return QuestEnemyFamily::Skeleton;
    }

    return QuestEnemyFamily::Any;
}

bool IsItemQuestValid(const QuestObjective& objective) {
    return objective.type != QuestObjectiveType::RetrieveItemOnFloor || objective.targetFloor > 0;
}

} // namespace

const std::vector<QuestDefinition>& GetAllQuestDefinitions() {
    static const std::vector<QuestDefinition> allQuests{
        {QuestId::GraveyardSweep,
         "Graveyard Sweep",
         "Defeat 8 Skeleton-type enemies.",
         "Clear the old catacomb route for city travelers.",
         "220 Gold, 1 Guild Seal",
         QuestObjective{QuestObjectiveType::DefeatEnemyFamily, 8, 1, QuestEnemyFamily::Skeleton, items::ItemId::GuildSeal, 0}},
        {QuestId::SlimeContainment,
         "Slime Containment",
         "Defeat 12 Slimes on floor 2+.",
         "Stop slime spread before it reaches the city gates.",
         "180 Gold, 2 Greater Potions",
         QuestObjective{QuestObjectiveType::DefeatEnemyFamily, 12, 2, QuestEnemyFamily::Slime, items::ItemId::GuildSeal, 0}},
        {QuestId::RelicRetrieval,
         "Relic Retrieval",
         "Recover an Ancient Relic from floor 4.",
         "Retrieve the scholar's proof item from deep ruins.",
         "320 Gold, Rare Guild Access",
         QuestObjective{QuestObjectiveType::RetrieveItemOnFloor, 1, 0, QuestEnemyFamily::Any, items::ItemId::AncientRelic, 4}},
        {QuestId::BrokenCaravan,
         "Broken Caravan",
         "Defeat 6 enemies while guarding floor 1 route.",
         "Secure the trade lane after repeated monster raids.",
         "140 Gold, Bronze Sword",
         QuestObjective{QuestObjectiveType::DefeatEnemyFamily, 6, 1, QuestEnemyFamily::Any, items::ItemId::GuildSeal, 0}},
        {QuestId::FloodedCatacombs,
         "Flooded Catacombs",
         "Recover a Guild Seal from floor 3.",
         "Bring back the missing insignia from flooded passages.",
         "360 Gold, 1 Elixir",
         QuestObjective{QuestObjectiveType::RetrieveItemOnFloor, 1, 0, QuestEnemyFamily::Any, items::ItemId::GuildSeal, 3}},
        {QuestId::NightWatch,
         "Night Watch",
         "Defeat 5 enemies on floor 5+.",
         "Prove combat readiness for the city guard rotation.",
         "200 Gold, Focus Amulet",
         QuestObjective{QuestObjectiveType::DefeatEnemyFamily, 5, 5, QuestEnemyFamily::Any, items::ItemId::GuildSeal, 0}},
    };

    return allQuests;
}

std::vector<QuestDefinition> GetRandomGuildQuestSubset(int maxCount) {
    const std::vector<QuestDefinition>& allQuests = GetAllQuestDefinitions();
    const int total = static_cast<int>(allQuests.size());
    const int wanted = std::clamp(maxCount, 0, total);

    std::vector<int> indices(static_cast<size_t>(total));
    std::iota(indices.begin(), indices.end(), 0);

    for (int i = total - 1; i > 0; --i) {
        const int j = GetRandomValue(0, i);
        std::swap(indices[static_cast<size_t>(i)], indices[static_cast<size_t>(j)]);
    }

    std::vector<QuestDefinition> result;
    result.reserve(static_cast<size_t>(wanted));
    for (int i = 0; i < wanted; ++i) {
        result.push_back(allQuests[static_cast<size_t>(indices[static_cast<size_t>(i)])]);
    }

    return result;
}

const QuestDefinition& GetQuestDefinition(QuestId id) {
    const std::vector<QuestDefinition>& allQuests = GetAllQuestDefinitions();
    const auto it = std::find_if(allQuests.begin(), allQuests.end(), [&](const QuestDefinition& q) {
        return q.id == id;
    });

    if (it != allQuests.end()) {
        return *it;
    }

    return allQuests.front();
}

QuestLog::QuestLog()
    : states(GetAllQuestDefinitions().size()) {}

bool QuestLog::AcceptQuest(QuestId id) {
    const size_t index = QuestIndex(id);
    if (index >= states.size()) {
        return false;
    }

    QuestState& state = states[index];
    if (state.accepted) {
        return false;
    }

    state.accepted = true;
    state.completed = false;
    state.progress = 0;
    return true;
}

bool QuestLog::IsAccepted(QuestId id) const {
    const size_t index = QuestIndex(id);
    return index < states.size() && states[index].accepted;
}

bool QuestLog::IsCompleted(QuestId id) const {
    const size_t index = QuestIndex(id);
    return index < states.size() && states[index].completed;
}

int QuestLog::GetProgress(QuestId id) const {
    const size_t index = QuestIndex(id);
    if (index >= states.size()) {
        return 0;
    }
    return states[index].progress;
}

int QuestLog::GetRequiredCount(QuestId id) const {
    return std::max(1, GetQuestDefinition(id).objective.requiredCount);
}

void QuestLog::RegisterBattleVictory(int floor, const std::vector<EnemyArchetype>& defeatedEnemies) {
    if (defeatedEnemies.empty()) {
        return;
    }

    const std::vector<QuestDefinition>& allQuests = GetAllQuestDefinitions();
    for (const QuestDefinition& quest : allQuests) {
        const size_t index = QuestIndex(quest.id);
        if (index >= states.size()) {
            continue;
        }

        QuestState& state = states[index];
        if (!state.accepted || state.completed) {
            continue;
        }

        const QuestObjective& objective = quest.objective;
        if (objective.type != QuestObjectiveType::DefeatEnemyFamily) {
            continue;
        }

        if (floor < objective.minFloor) {
            continue;
        }

        int matchingKills = 0;
        for (EnemyArchetype enemy : defeatedEnemies) {
            const QuestEnemyFamily family = GetEnemyFamily(enemy);
            if (objective.enemyFamily == QuestEnemyFamily::Any || objective.enemyFamily == family) {
                matchingKills++;
            }
        }

        state.progress = std::min(objective.requiredCount, state.progress + matchingKills);
        if (state.progress >= objective.requiredCount) {
            state.completed = true;
        }
    }
}

void QuestLog::RegisterQuestItemCollected(QuestId questId, items::ItemId itemId, int floor) {
    const size_t index = QuestIndex(questId);
    if (index >= states.size()) {
        return;
    }

    QuestState& state = states[index];
    if (!state.accepted || state.completed) {
        return;
    }

    const QuestDefinition& quest = GetQuestDefinition(questId);
    const QuestObjective& objective = quest.objective;
    if (objective.type != QuestObjectiveType::RetrieveItemOnFloor) {
        return;
    }

    if (!IsItemQuestValid(objective)) {
        return;
    }

    if (objective.requiredItem != itemId || objective.targetFloor != floor) {
        return;
    }

    state.progress = std::min(objective.requiredCount, state.progress + 1);
    if (state.progress >= objective.requiredCount) {
        state.completed = true;
    }
}

std::vector<QuestItemSpawn> QuestLog::GetPendingQuestItemSpawnsForFloor(int floor) const {
    std::vector<QuestItemSpawn> spawns;
    const std::vector<QuestDefinition>& allQuests = GetAllQuestDefinitions();

    for (const QuestDefinition& quest : allQuests) {
        const size_t index = QuestIndex(quest.id);
        if (index >= states.size()) {
            continue;
        }

        const QuestState& state = states[index];
        if (!state.accepted || state.completed) {
            continue;
        }

        const QuestObjective& objective = quest.objective;
        if (objective.type != QuestObjectiveType::RetrieveItemOnFloor) {
            continue;
        }

        if (!IsItemQuestValid(objective)) {
            continue;
        }

        if (objective.targetFloor != floor) {
            continue;
        }

        if (state.progress >= objective.requiredCount) {
            continue;
        }

        spawns.push_back(QuestItemSpawn{quest.id, objective.requiredItem, objective.targetFloor});
    }

    return spawns;
}

size_t QuestLog::QuestIndex(QuestId id) {
    return static_cast<size_t>(id);
}

} // namespace quests
