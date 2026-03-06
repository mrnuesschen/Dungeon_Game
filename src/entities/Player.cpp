#include "entities/Player.h"

#include <algorithm>
#include <cstddef>

#include "raylib.h"

Player::Player()
        : Entity("Hero", "", 1),
            playerClass(PlayerClass::Knight),
            className("Knight"),
            characterName("Hero"),
            avatarPath(""),
      attackMin(16),
      attackMax(28),
      healMin(10),
      healMax(18),
            exp(0),
        gold(0),
      defending(false) {
    ApplyPreset();
        SetHp(GetMaxHp());
}

void Player::SetClass(PlayerClass value) {
    playerClass = value;
    ApplyPreset();
}

PlayerClass Player::GetClass() const {
    return playerClass;
}

int Player::GetLevel() const {
    return Entity::GetLevel();
}

int Player::GetExp() const {
    return exp;
}

int Player::GetExpToNextLevel() const {
    return CalculateExpRequiredForLevel(GetLevel());
}

int Player::GetGold() const {
    return gold;
}

void Player::SetGold(int value) {
    gold = std::max(0, value);
}

void Player::AddGold(int amount) {
    if (amount > 0) {
        gold += amount;
    }
}

bool Player::SpendGold(int amount) {
    if (amount <= 0 || gold < amount) {
        return false;
    }

    gold -= amount;
    return true;
}

int Player::AddExperience(int amount) {
    if (amount <= 0) {
        return 0;
    }

    exp += amount;
    int levelUps = 0;
    while (exp >= GetExpToNextLevel()) {
        exp -= GetExpToNextLevel();
        SetLevelInternal(GetLevel() + 1);
        ApplyPreset();
        levelUps++;
    }

    return levelUps;
}

void Player::SetProgress(int newLevel, int newExp) {
    SetLevelInternal(std::max(1, newLevel));
    exp = std::max(0, newExp);

    int nextThreshold = CalculateExpRequiredForLevel(GetLevel());
    while (exp >= nextThreshold) {
        exp -= nextThreshold;
        SetLevelInternal(GetLevel() + 1);
        nextThreshold = CalculateExpRequiredForLevel(GetLevel());
    }

    ApplyPreset();
}

void Player::SetCharacterName(const std::string& value) {
    if (value.empty()) {
        characterName = "Hero";
        return;
    }

    characterName = value;
}

const std::string& Player::GetCharacterName() const {
    return characterName;
}

void Player::SetAvatarPath(const std::string& value) {
    avatarPath = value;
}

const std::string& Player::GetAvatarPath() const {
    return avatarPath;
}

void Player::ResetForBattle() {
    SetHp(GetMaxHp());
    defending = false;
}

int Player::RollAttack() const {
    return Entity::RollAttack();
}

int Player::RollHeal() const {
    return GetRandomValue(healMin, healMax);
}

bool Player::IsDefending() const {
    return defending;
}

void Player::SetDefending(bool value) {
    defending = value;
}

void Player::AddItemToInventory(items::ItemId id, int quantity) {
    inventory.AddItem(id, quantity);
}

bool Player::CanAddItemToInventory(items::ItemId id, int quantity) const {
    return inventory.CanAddItem(id, quantity);
}

void Player::SetInventoryEntries(const std::vector<items::InventoryEntry>& entries) {
    inventory.SetEntries(entries);
}

const items::Inventory& Player::GetInventory() const {
    return inventory;
}

const std::string& Player::GetClassName() const {
    return className;
}

const char* Player::ClassToString(PlayerClass value) {
    switch (value) {
    case PlayerClass::Tank:
        return "Tank";
    case PlayerClass::Knight:
        return "Knight";
    case PlayerClass::Assassin:
        return "Assassin";
    case PlayerClass::Mage:
        return "Mage";
    case PlayerClass::Healer:
        return "Healer";
    case PlayerClass::Alchemist:
        return "Alchemist";
    }
    return "Knight";
}

void Player::ApplyPreset() {
    const int previousHp = GetHp();

    struct PlayerScaling {
        const char* className;
        int baseHp;
        int hpPerLevel;
        int baseAttackMin;
        int attackMinPerLevel;
        int baseAttackMax;
        int attackMaxPerLevel;
        int baseHealMin;
        int healMinPerLevel;
        int baseHealMax;
        int healMaxPerLevel;
    };

    PlayerScaling scaling{};

    switch (playerClass) {
    case PlayerClass::Tank:
        scaling = PlayerScaling{"Tank", 340, 44, 24, 4, 40, 4, 16, 2, 28, 2};
        break;
    case PlayerClass::Knight:
        scaling = PlayerScaling{"Knight", 260, 36, 32, 4, 56, 6, 20, 2, 36, 4};
        break;
    case PlayerClass::Assassin:
        scaling = PlayerScaling{"Assassin", 196, 24, 44, 6, 72, 6, 16, 2, 28, 2};
        break;
    case PlayerClass::Mage:
        scaling = PlayerScaling{"Mage", 208, 28, 40, 6, 68, 6, 24, 2, 40, 4};
        break;
    case PlayerClass::Healer:
        scaling = PlayerScaling{"Healer", 236, 32, 24, 4, 44, 4, 36, 4, 60, 4};
        break;
    case PlayerClass::Alchemist:
        scaling = PlayerScaling{"Alchemist", 224, 30, 28, 4, 52, 6, 28, 4, 48, 4};
        break;
    }

    className = scaling.className;

    const int levelOffset = GetLevel() - 1;
    const int computedMaxHp = scaling.baseHp + scaling.hpPerLevel * levelOffset;
    attackMin = scaling.baseAttackMin + scaling.attackMinPerLevel * levelOffset;
    attackMax = scaling.baseAttackMax + scaling.attackMaxPerLevel * levelOffset;
    healMin = scaling.baseHealMin + scaling.healMinPerLevel * levelOffset;
    healMax = scaling.baseHealMax + scaling.healMaxPerLevel * levelOffset;

    SetStats(computedMaxHp, attackMin, attackMax);

    const auto& classSkills = combat::GetSkillsForClass(playerClass);
    size_t unlockedSkillCount = 2;
    if (GetLevel() >= 5) {
        unlockedSkillCount = 4;
    } else if (GetLevel() >= 3) {
        unlockedSkillCount = 3;
    }
    unlockedSkillCount = std::min(unlockedSkillCount, classSkills.size());

    SetSkills(std::vector<combat::SkillDefinition>(classSkills.begin(), classSkills.begin() + static_cast<std::ptrdiff_t>(unlockedSkillCount)));

    if (characterName.empty()) {
        characterName = "Hero";
    }

    if (previousHp <= 0) {
        SetHp(GetMaxHp());
    }
}

int Player::CalculateExpRequiredForLevel(int levelValue) const {
    const int clampedLevel = std::max(1, levelValue);
    return 120 + (clampedLevel - 1) * 55;
}
