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
            attack(16),
            defense(6),
      healMin(10),
      healMax(18),
            exp(0),
        gold(0),
      mana(40),
      maxMana(40),
      hpRegen(0),
      manaRegen(0),
    defending(false),
    inventory(),
    equipmentLoadout() {
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

int Player::GetMana() const {
    return mana;
}

int Player::GetMaxMana() const {
    return maxMana;
}

int Player::GetMagicAttack() const {
    return Entity::GetMagicAttack();
}

int Player::GetMagicDefense() const {
    return Entity::GetMagicDefense();
}

int Player::GetHpRegen() const {
    return hpRegen;
}

int Player::GetManaRegen() const {
    return manaRegen;
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

bool Player::SpendMana(int amount) {
    if (amount <= 0) {
        return true;
    }

    if (mana < amount) {
        return false;
    }

    mana -= amount;
    return true;
}

void Player::RestoreMana(int amount) {
    if (amount <= 0) {
        return;
    }

    mana = std::min(maxMana, mana + amount);
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
    mana = maxMana;
    defending = false;
}

void Player::ApplyTurnRegeneration() {
    if (hpRegen > 0) {
        Heal(hpRegen);
    }
    if (manaRegen > 0) {
        RestoreMana(manaRegen);
    }
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

bool Player::EquipItem(items::ItemId id, items::EquipmentSlot slot, std::string& outMessage) {
    if (!items::CanEquipInSlot(id, slot)) {
        outMessage = std::string("Item cannot be equipped in slot: ") + items::EquipmentSlotToString(slot) + ".";
        return false;
    }

    const int availableCopies = inventory.GetQuantity(id);
    const int alreadyEquippedCopies = equipmentLoadout.CountEquippedCopies(id);
    if (availableCopies <= alreadyEquippedCopies) {
        outMessage = "You do not have a free copy of this item to equip.";
        return false;
    }

    if (!equipmentLoadout.Equip(id, slot)) {
        outMessage = "Failed to equip item.";
        return false;
    }

    ApplyPreset();
    outMessage = std::string("Equipped ") + items::GetItemDefinition(id).name + " in " + items::EquipmentSlotToString(slot) + ".";
    return true;
}

bool Player::UnequipItem(items::EquipmentSlot slot, std::string& outMessage) {
    const std::optional<items::ItemId> equipped = equipmentLoadout.GetEquippedItem(slot);
    if (!equipped.has_value()) {
        outMessage = "Slot is already empty.";
        return false;
    }

    if (!equipmentLoadout.Unequip(slot)) {
        outMessage = "Failed to unequip item.";
        return false;
    }

    ApplyPreset();
    outMessage = std::string("Unequipped ") + items::GetItemDefinition(*equipped).name + " from " + items::EquipmentSlotToString(slot) + ".";
    return true;
}

const items::EquipmentLoadout& Player::GetEquipmentLoadout() const {
    return equipmentLoadout;
}

void Player::SetInventoryEntries(const std::vector<items::InventoryEntry>& entries) {
    inventory.SetEntries(entries);
    equipmentLoadout.Clear();
    ApplyPreset();
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
        int baseMana;
        int manaPerLevel;
        int baseAttack;
        int attackPerLevel;
        int baseDefense;
        int defensePerLevel;
        int baseMagicAttack;
        int magicAttackPerLevel;
        int baseMagicDefense;
        int magicDefensePerLevel;
        int baseHealMin;
        int healMinPerLevel;
        int baseHealMax;
        int healMaxPerLevel;
        int baseHpRegen;
        int hpRegenPerLevel;
        int baseManaRegen;
        int manaRegenPerLevel;
    };

    PlayerScaling scaling{};

    switch (playerClass) {
    case PlayerClass::Tank:
        scaling = PlayerScaling{"Tank", 340, 44, 36, 3, 32, 4, 12, 2, 12, 2, 18, 2, 16, 2, 28, 2, 5, 1, 3, 1};
        break;
    case PlayerClass::Knight:
        scaling = PlayerScaling{"Knight", 260, 36, 42, 3, 44, 5, 9, 1, 15, 2, 16, 2, 20, 2, 36, 4, 4, 1, 3, 1};
        break;
    case PlayerClass::Assassin:
        scaling = PlayerScaling{"Assassin", 196, 24, 38, 2, 58, 6, 5, 1, 14, 2, 12, 1, 16, 2, 28, 2, 3, 1, 4, 1};
        break;
    case PlayerClass::Mage:
        scaling = PlayerScaling{"Mage", 208, 28, 64, 5, 54, 6, 6, 1, 24, 4, 14, 2, 24, 2, 40, 4, 3, 1, 6, 1};
        break;
    case PlayerClass::Healer:
        scaling = PlayerScaling{"Healer", 236, 32, 60, 5, 34, 4, 8, 1, 22, 3, 16, 2, 36, 4, 60, 4, 4, 1, 6, 1};
        break;
    case PlayerClass::Alchemist:
        scaling = PlayerScaling{"Alchemist", 224, 30, 54, 4, 40, 5, 8, 1, 21, 3, 15, 2, 28, 4, 48, 4, 4, 1, 5, 1};
        break;
    }

    className = scaling.className;

    const int levelOffset = GetLevel() - 1;
    int computedMaxHp = scaling.baseHp + scaling.hpPerLevel * levelOffset;
    int computedMaxMana = scaling.baseMana + scaling.manaPerLevel * levelOffset;
    int computedAttack = scaling.baseAttack + scaling.attackPerLevel * levelOffset;
    int computedDefense = scaling.baseDefense + scaling.defensePerLevel * levelOffset;
    int computedMagicAttack = scaling.baseMagicAttack + scaling.magicAttackPerLevel * levelOffset;
    int computedMagicDefense = scaling.baseMagicDefense + scaling.magicDefensePerLevel * levelOffset;
    int computedHealMin = scaling.baseHealMin + scaling.healMinPerLevel * levelOffset;
    int computedHealMax = scaling.baseHealMax + scaling.healMaxPerLevel * levelOffset;
    int computedHpRegen = scaling.baseHpRegen + scaling.hpRegenPerLevel * levelOffset;
    int computedManaRegen = scaling.baseManaRegen + scaling.manaRegenPerLevel * levelOffset;

    for (const items::EquipmentSlot slot : equipmentLoadout.GetOccupiedSlots()) {
        const std::optional<items::ItemId> equipped = equipmentLoadout.GetEquippedItem(slot);
        if (!equipped.has_value()) {
            continue;
        }

        const items::ItemDefinition::StatBonuses& bonuses = items::GetItemDefinition(*equipped).statBonuses;
        computedMaxHp += bonuses.maxHp;
        computedAttack += (bonuses.attackMin + bonuses.attackMax) / 2;
        computedDefense += bonuses.defense;
        computedHealMin += bonuses.healMin;
        computedHealMax += bonuses.healMax;
        computedMaxMana += bonuses.maxMana;
        computedMagicAttack += bonuses.magicAttack;
        computedMagicDefense += bonuses.magicDefense;
        computedHpRegen += bonuses.hpRegen;
        computedManaRegen += bonuses.manaRegen;
    }

    computedMaxHp = std::max(1, computedMaxHp);
    computedMaxMana = std::max(1, computedMaxMana);
    computedAttack = std::max(1, computedAttack);
    computedDefense = std::max(0, computedDefense);
    computedMagicAttack = std::max(0, computedMagicAttack);
    computedMagicDefense = std::max(0, computedMagicDefense);
    computedHealMin = std::max(1, computedHealMin);
    computedHealMax = std::max(computedHealMin, computedHealMax);
    computedHpRegen = std::max(0, computedHpRegen);
    computedManaRegen = std::max(0, computedManaRegen);

    SetStats(computedMaxHp, computedAttack, computedDefense, computedMagicAttack, computedMagicDefense);
    attack = computedAttack;
    defense = computedDefense;
    healMin = computedHealMin;
    healMax = computedHealMax;
    maxMana = computedMaxMana;
    mana = std::clamp(mana, 0, maxMana);
    hpRegen = computedHpRegen;
    manaRegen = computedManaRegen;

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
