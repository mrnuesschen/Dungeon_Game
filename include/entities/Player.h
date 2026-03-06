#pragma once

#include <string>
#include <vector>

#include "GameTypes.h"
#include "entities/Entity.h"
#include "combat/SkillSystem.h"
#include "items/EquipmentLoadout.h"
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
    int GetMana() const;
    int GetMaxMana() const;
    int GetMagicAttack() const;
    int GetMagicDefense() const;
    int GetHpRegen() const;
    int GetManaRegen() const;
    int AddExperience(int amount);
    void SetProgress(int newLevel, int newExp);
    int GetGold() const;
    void SetGold(int value);
    void AddGold(int amount);
    bool SpendGold(int amount);
    bool SpendMana(int amount);
    void RestoreMana(int amount);

    void SetCharacterName(const std::string& value);
    const std::string& GetCharacterName() const;

    void SetAvatarPath(const std::string& value);
    const std::string& GetAvatarPath() const;

    void ResetForBattle();
    void ApplyTurnRegeneration();

    int RollAttack() const;
    int RollHeal() const;

    bool IsDefending() const;
    void SetDefending(bool defending);
    void AddItemToInventory(items::ItemId id, int quantity = 1);
    bool CanAddItemToInventory(items::ItemId id, int quantity = 1) const;
    bool EquipItem(items::ItemId id, items::EquipmentSlot slot, std::string& outMessage);
    bool UnequipItem(items::EquipmentSlot slot, std::string& outMessage);
    const items::EquipmentLoadout& GetEquipmentLoadout() const;
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
    int attack;
    int defense;
    int healMin;
    int healMax;
    int exp;
    int gold;
    int mana;
    int maxMana;
    int hpRegen;
    int manaRegen;
    bool defending;
    items::Inventory inventory;
    items::EquipmentLoadout equipmentLoadout;
};
