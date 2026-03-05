#pragma once

#include <array>
#include <string>

enum class AppScene {
    MainMenu,
    CharacterCreation,
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

constexpr int kMaxBattleEnemies = 4;

struct CharacterSetupData {
    PlayerClass playerClass = PlayerClass::Knight;
    std::string playerName = "Hero";
    std::string avatarPath;
};

struct BattleSaveData {
    int playerHp = 120;
    std::array<int, kMaxBattleEnemies> enemyHp{140, 0, 0, 0};
    int enemyCount = 1;
    bool playerDefending = false;
    PlayerClass playerClass = PlayerClass::Knight;
    std::array<EnemyArchetype, kMaxBattleEnemies> enemyType{
        EnemyArchetype::SmallSlime,
        EnemyArchetype::SmallSlime,
        EnemyArchetype::SmallSlime,
        EnemyArchetype::SmallSlime};
    BattlePhase phase = BattlePhase::PlayerTurn;
    std::string playerName = "Hero";
    std::string avatarPath;
};

struct GalleryData {
    std::array<bool, 4> unlocked{false, false, false, false};
};
