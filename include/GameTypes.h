#pragma once

#include <array>
#include <string>

#include "items/ItemSystem.h"

enum class AppScene {
    MainMenu,
    CharacterCreation,
    WorldMap,
    CityMap,
    Dungeon,
    Battle,
    Gallery
};

enum class BattlePhase {
    PlayerTurn,
    EnemyTurn,
    Won,
    Lost
};

enum class PlayerClass {
    Tank,
    Knight,
    Assassin,
    Mage,
    Healer,
    Alchemist
};

enum class EnemyArchetype {
    SmallSlime,
    MediumSlime,
    LargeSlime,
    LesserSkeleton,
    Skeleton,
    SkeletonWarrior
};

constexpr int kMaxBattleEnemies = 8;
constexpr int kMaxInventorySaveEntries = 48;
constexpr int kMaxEquipmentSaveEntries = static_cast<int>(items::EquipmentSlot::Count);

struct CharacterSetupData {
    PlayerClass playerClass = PlayerClass::Knight;
    std::string playerName = "Hero";
    std::string avatarPath;
};

struct BattleSaveData {
    int playerHp = 120;
    int playerLevel = 1;
    int playerExp = 0;
    int playerGold = 0;
    std::array<int, kMaxBattleEnemies> enemyHp{140, 0, 0, 0};
    std::array<int, kMaxBattleEnemies> enemyLevel{1, 1, 1, 1, 1, 1, 1, 1};
    int enemyCount = 1;
    bool playerDefending = false;
    PlayerClass playerClass = PlayerClass::Knight;
    std::array<EnemyArchetype, kMaxBattleEnemies> enemyType{
        EnemyArchetype::SmallSlime,
        EnemyArchetype::SmallSlime,
        EnemyArchetype::SmallSlime,
        EnemyArchetype::SmallSlime,
        EnemyArchetype::SmallSlime,
        EnemyArchetype::SmallSlime,
        EnemyArchetype::SmallSlime,
        EnemyArchetype::SmallSlime};
    int inventoryEntryCount = 0;
    std::array<int, kMaxInventorySaveEntries> inventoryItemId{};
    std::array<int, kMaxInventorySaveEntries> inventoryQuantity{};
    int equippedEntryCount = 0;
    std::array<int, kMaxEquipmentSaveEntries> equippedSlotIndex{};
    std::array<int, kMaxEquipmentSaveEntries> equippedItemId{};
    BattlePhase phase = BattlePhase::PlayerTurn;
    std::string playerName = "Hero";
    std::string avatarPath;
};

struct SaveSlotPreview {
    int slotIndex = 0;
    std::string label;
};

struct GalleryData {
    std::array<bool, 4> unlocked{false, false, false, false};
};
