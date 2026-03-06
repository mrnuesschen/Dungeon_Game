#pragma once

#include <string>
#include <vector>

#include "GameTypes.h"
#include "entities/Entity.h"
#include "combat/SkillSystem.h"
#include "items/Inventory.h"
#include "items/ItemSystem.h"

class Player : public Entity {
public:
    Player();

    void SetClass(PlayerClass playerClass);
    PlayerClass GetClass() const;

    int GetLevel() const;
    int GetExp() const;
    int GetExpToNextLevel() const;
    int AddExperience(int amount);
    void SetProgress(int newLevel, int newExp);
    int GetGold() const;
    void SetGold(int value);
    void AddGold(int amount);
    bool SpendGold(int amount);

    void SetCharacterName(const std::string& value);
    const std::string& GetCharacterName() const;

    void SetAvatarPath(const std::string& value);
    const std::string& GetAvatarPath() const;

    void ResetForBattle();

    int RollAttack() const;
    int RollHeal() const;

    bool IsDefending() const;
    void SetDefending(bool defending);
    void AddItemToInventory(items::ItemId id, int quantity = 1);
    bool CanAddItemToInventory(items::ItemId id, int quantity = 1) const;
    void SetInventoryEntries(const std::vector<items::InventoryEntry>& entries);
    const items::Inventory& GetInventory() const;

    const std::string& GetClassName() const;

    static const char* ClassToString(PlayerClass playerClass);

private:
    void ApplyPreset();
    int CalculateExpRequiredForLevel(int levelValue) const;

    PlayerClass playerClass;
    std::string className;
    std::string characterName;
    std::string avatarPath;
    int attackMin;
    int attackMax;
    int healMin;
    int healMax;
    int exp;
    int gold;
    bool defending;
    items::Inventory inventory;
};
