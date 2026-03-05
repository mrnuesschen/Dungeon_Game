#include "SaveSystem.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <string>

namespace {

constexpr const char* kBattleSavePath = "savegame.txt";
constexpr const char* kGallerySavePath = "gallery.txt";

int ToInt(BattlePhase phase) {
    switch (phase) {
    case BattlePhase::PlayerTurn:
        return 0;
    case BattlePhase::EnemyTurn:
        return 1;
    case BattlePhase::Won:
        return 2;
    case BattlePhase::Lost:
        return 3;
    }
    return 0;
}

int ToInt(PlayerClass playerClass) {
    switch (playerClass) {
    case PlayerClass::Tank:
        return 0;
    case PlayerClass::Knight:
        return 1;
    case PlayerClass::Assassin:
        return 2;
    case PlayerClass::Mage:
        return 3;
    case PlayerClass::Healer:
        return 4;
    case PlayerClass::Alchemist:
        return 5;
    }
    return 1;
}

int ToInt(EnemyArchetype archetype) {
    switch (archetype) {
    case EnemyArchetype::SmallSlime:
        return 0;
    case EnemyArchetype::MediumSlime:
        return 1;
    case EnemyArchetype::LargeSlime:
        return 2;
    case EnemyArchetype::LesserSkeleton:
        return 3;
    case EnemyArchetype::Skeleton:
        return 4;
    case EnemyArchetype::SkeletonWarrior:
        return 5;
    }
    return 0;
}

BattlePhase ToPhase(int value) {
    switch (value) {
    case 0:
        return BattlePhase::PlayerTurn;
    case 1:
        return BattlePhase::EnemyTurn;
    case 2:
        return BattlePhase::Won;
    case 3:
        return BattlePhase::Lost;
    default:
        return BattlePhase::PlayerTurn;
    }
}

PlayerClass ToPlayerClass(int value) {
    switch (value) {
    case 0:
        return PlayerClass::Tank;
    case 1:
        return PlayerClass::Knight;
    case 2:
        return PlayerClass::Assassin;
    case 3:
        return PlayerClass::Mage;
    case 4:
        return PlayerClass::Healer;
    case 5:
        return PlayerClass::Alchemist;
    default:
        return PlayerClass::Knight;
    }
}

EnemyArchetype ToEnemyArchetype(int value) {
    switch (value) {
    case 0:
        return EnemyArchetype::SmallSlime;
    case 1:
        return EnemyArchetype::MediumSlime;
    case 2:
        return EnemyArchetype::LargeSlime;
    case 3:
        return EnemyArchetype::LesserSkeleton;
    case 4:
        return EnemyArchetype::Skeleton;
    case 5:
        return EnemyArchetype::SkeletonWarrior;
    default:
        return EnemyArchetype::SmallSlime;
    }
}

}

namespace save_system {

bool SaveBattle(const BattleSaveData& data) {
    std::ofstream out(kBattleSavePath, std::ios::trunc);
    if (!out.is_open()) {
        return false;
    }

    out << data.playerHp << '\n';
    out << data.enemyCount << '\n';
    out << (data.playerDefending ? 1 : 0) << '\n';
    out << ToInt(data.playerClass) << '\n';
    out << ToInt(data.phase) << '\n';
    out << data.playerName << '\n';
    out << data.avatarPath << '\n';

    const int clampedCount = std::clamp(data.enemyCount, 1, kMaxBattleEnemies);
    for (int i = 0; i < clampedCount; ++i) {
        out << ToInt(data.enemyType[static_cast<size_t>(i)]) << ' ' << data.enemyHp[static_cast<size_t>(i)] << '\n';
    }

    return out.good();
}

bool LoadBattle(BattleSaveData& outData) {
    std::ifstream in(kBattleSavePath);
    if (!in.is_open()) {
        return false;
    }

    int defending = 0;
    int playerClass = 1;
    int phase = 0;

    if (!(in >> outData.playerHp >> outData.enemyCount >> defending >> playerClass >> phase)) {
        return false;
    }

    std::string ignored;
    std::getline(in, ignored);
    std::getline(in, outData.playerName);
    std::getline(in, outData.avatarPath);

    if (outData.playerName.empty()) {
        outData.playerName = "Hero";
    }

    outData.playerDefending = defending != 0;
    outData.playerClass = ToPlayerClass(playerClass);
    outData.phase = ToPhase(phase);

    const int clampedCount = std::clamp(outData.enemyCount, 1, kMaxBattleEnemies);
    outData.enemyCount = clampedCount;

    for (int i = 0; i < clampedCount; ++i) {
        int enemyType = 0;
        int enemyHp = 1;
        if (!(in >> enemyType >> enemyHp)) {
            return false;
        }

        outData.enemyType[static_cast<size_t>(i)] = ToEnemyArchetype(enemyType);
        outData.enemyHp[static_cast<size_t>(i)] = std::max(0, enemyHp);
    }

    for (int i = clampedCount; i < kMaxBattleEnemies; ++i) {
        outData.enemyType[static_cast<size_t>(i)] = EnemyArchetype::SmallSlime;
        outData.enemyHp[static_cast<size_t>(i)] = 0;
    }

    return true;
}

bool BattleSaveExists() {
    std::ifstream in(kBattleSavePath);
    return in.good();
}

void DeleteBattleSave() {
    std::remove(kBattleSavePath);
}

bool SaveGallery(const GalleryData& data) {
    std::ofstream out(kGallerySavePath, std::ios::trunc);
    if (!out.is_open()) {
        return false;
    }

    for (bool unlocked : data.unlocked) {
        out << (unlocked ? 1 : 0) << '\n';
    }

    return out.good();
}

GalleryData LoadGallery() {
    GalleryData data;

    std::ifstream in(kGallerySavePath);
    if (!in.is_open()) {
        return data;
    }

    for (bool& unlocked : data.unlocked) {
        int value = 0;
        if (!(in >> value)) {
            break;
        }
        unlocked = value != 0;
    }

    return data;
}

}
