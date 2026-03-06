#include "SaveSystem.h"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "entities/Player.h"
#include "items/ItemSystem.h"

namespace {

constexpr const char* kBattleSavePath = "savegame.txt";
constexpr const char* kGallerySavePath = "gallery.txt";
constexpr const char* kSaveDirectory = "saves";

std::string MakeSlotPath(int slotIndex) {
    return std::string(kSaveDirectory) + "/slot_" + std::to_string(slotIndex) + ".txt";
}

bool IsValidSlotIndex(int slotIndex) {
    return slotIndex >= 1 && slotIndex <= save_system::kMaxGameSaveSlots;
}

void EnsureSaveDirectory() {
    std::error_code ec;
    std::filesystem::create_directories(kSaveDirectory, ec);
}

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

int ToInt(items::ItemId id) {
    switch (id) {
    case items::ItemId::BronzeSword:
        return 0;
    case items::ItemId::IronSword:
        return 1;
    case items::ItemId::ReinforcedArmor:
        return 2;
    case items::ItemId::FocusAmulet:
        return 3;
    case items::ItemId::HealthPotion:
        return 4;
    case items::ItemId::GreaterHealthPotion:
        return 5;
    case items::ItemId::Elixir:
        return 6;
    case items::ItemId::AncientRelic:
        return 7;
    case items::ItemId::GuildSeal:
        return 8;
    }

    return 4;
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

items::ItemId ToItemId(int value) {
    switch (value) {
    case 0:
        return items::ItemId::BronzeSword;
    case 1:
        return items::ItemId::IronSword;
    case 2:
        return items::ItemId::ReinforcedArmor;
    case 3:
        return items::ItemId::FocusAmulet;
    case 4:
        return items::ItemId::HealthPotion;
    case 5:
        return items::ItemId::GreaterHealthPotion;
    case 6:
        return items::ItemId::Elixir;
    case 7:
        return items::ItemId::AncientRelic;
    case 8:
        return items::ItemId::GuildSeal;
    default:
        return items::ItemId::HealthPotion;
    }
}

bool SaveBattleToPath(const BattleSaveData& data, const std::string& path) {
    std::ofstream out(path, std::ios::trunc);
    if (!out.is_open()) {
        return false;
    }

    out << data.playerHp << '\n';
    out << data.enemyCount << '\n';
    out << (data.playerDefending ? 1 : 0) << '\n';
    out << ToInt(data.playerClass) << '\n';
    out << ToInt(data.phase) << '\n';
    out << data.playerLevel << '\n';
    out << data.playerExp << '\n';
    out << data.playerGold << '\n';
    out << data.playerName << '\n';
    out << data.avatarPath << '\n';

    const int clampedCount = std::clamp(data.enemyCount, 1, kMaxBattleEnemies);
    for (int i = 0; i < clampedCount; ++i) {
        out << ToInt(data.enemyType[static_cast<size_t>(i)]) << ' ' << data.enemyHp[static_cast<size_t>(i)] << ' ' << data.enemyLevel[static_cast<size_t>(i)] << '\n';
    }

    const int clampedInventoryCount = std::clamp(data.inventoryEntryCount, 0, kMaxInventorySaveEntries);
    out << clampedInventoryCount << '\n';
    for (int i = 0; i < clampedInventoryCount; ++i) {
        out << data.inventoryItemId[static_cast<size_t>(i)] << ' ' << data.inventoryQuantity[static_cast<size_t>(i)] << '\n';
    }

    return out.good();
}

bool LoadBattleFromPath(BattleSaveData& outData, const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        return false;
    }

    int defending = 0;
    int playerClass = 1;
    int phase = 0;
    int playerLevel = 1;
    int playerExp = 0;
    int playerGold = 0;

    if (!(in >> outData.playerHp >> outData.enemyCount >> defending >> playerClass >> phase >> playerLevel >> playerExp >> playerGold)) {
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
    outData.playerLevel = std::max(1, playerLevel);
    outData.playerExp = std::max(0, playerExp);
    outData.playerGold = std::max(0, playerGold);

    const int clampedCount = std::clamp(outData.enemyCount, 1, kMaxBattleEnemies);
    outData.enemyCount = clampedCount;

    for (int i = 0; i < clampedCount; ++i) {
        int enemyType = 0;
        int enemyHp = 1;
        int enemyLevel = 1;
        if (!(in >> enemyType >> enemyHp >> enemyLevel)) {
            return false;
        }

        outData.enemyType[static_cast<size_t>(i)] = ToEnemyArchetype(enemyType);
        outData.enemyHp[static_cast<size_t>(i)] = std::max(0, enemyHp);
        outData.enemyLevel[static_cast<size_t>(i)] = std::max(1, enemyLevel);
    }

    for (int i = clampedCount; i < kMaxBattleEnemies; ++i) {
        outData.enemyType[static_cast<size_t>(i)] = EnemyArchetype::SmallSlime;
        outData.enemyHp[static_cast<size_t>(i)] = 0;
        outData.enemyLevel[static_cast<size_t>(i)] = 1;
    }

    int inventoryCount = 0;
    if (!(in >> inventoryCount)) {
        outData.inventoryEntryCount = 0;
        outData.inventoryItemId.fill(0);
        outData.inventoryQuantity.fill(0);
        return true;
    }

    const int clampedInventoryCount = std::clamp(inventoryCount, 0, kMaxInventorySaveEntries);
    outData.inventoryEntryCount = clampedInventoryCount;
    for (int i = 0; i < clampedInventoryCount; ++i) {
        int itemId = 0;
        int quantity = 0;
        if (!(in >> itemId >> quantity)) {
            outData.inventoryEntryCount = i;
            break;
        }

        outData.inventoryItemId[static_cast<size_t>(i)] = ToInt(ToItemId(itemId));
        outData.inventoryQuantity[static_cast<size_t>(i)] = std::max(1, quantity);
    }

    for (int i = outData.inventoryEntryCount; i < kMaxInventorySaveEntries; ++i) {
        outData.inventoryItemId[static_cast<size_t>(i)] = 0;
        outData.inventoryQuantity[static_cast<size_t>(i)] = 0;
    }

    return true;
}

std::string BuildSaveLabel(const BattleSaveData& data, int slotIndex) {
    return "Slot " + std::to_string(slotIndex) + " | " + data.playerName +
        " | " + std::string(Player::ClassToString(data.playerClass)) +
        " Lv " + std::to_string(data.playerLevel) +
        " | Gold " + std::to_string(data.playerGold);
}

}

namespace save_system {

bool SaveBattle(const BattleSaveData& data) {
    return SaveBattleToPath(data, kBattleSavePath);
}

bool LoadBattle(BattleSaveData& outData) {
    return LoadBattleFromPath(outData, kBattleSavePath);
}

bool BattleSaveExists() {
    std::ifstream in(kBattleSavePath);
    return in.good();
}

void DeleteBattleSave() {
    std::remove(kBattleSavePath);
}

int CreateInnSave(const BattleSaveData& data) {
    EnsureSaveDirectory();

    for (int slot = 1; slot <= kMaxGameSaveSlots; ++slot) {
        const std::string path = MakeSlotPath(slot);
        std::ifstream in(path);
        if (in.good()) {
            continue;
        }

        if (SaveBattleToPath(data, path)) {
            return slot;
        }

        return -1;
    }

    return -1;
}

bool LoadGameSlot(int slotIndex, BattleSaveData& outData) {
    if (!IsValidSlotIndex(slotIndex)) {
        return false;
    }

    return LoadBattleFromPath(outData, MakeSlotPath(slotIndex));
}

bool DeleteGameSlot(int slotIndex) {
    if (!IsValidSlotIndex(slotIndex)) {
        return false;
    }

    const std::string path = MakeSlotPath(slotIndex);
    return std::remove(path.c_str()) == 0;
}

std::vector<SaveSlotPreview> ListGameSaves() {
    std::vector<SaveSlotPreview> slots;
    slots.reserve(kMaxGameSaveSlots);

    for (int slot = 1; slot <= kMaxGameSaveSlots; ++slot) {
        BattleSaveData data;
        if (!LoadGameSlot(slot, data)) {
            continue;
        }

        SaveSlotPreview preview;
        preview.slotIndex = slot;
        preview.label = BuildSaveLabel(data, slot);
        slots.push_back(preview);
    }

    return slots;
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
