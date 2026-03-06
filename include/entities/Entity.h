#pragma once

#include <string>
#include <vector>

#include "combat/SkillSystem.h"

class Entity {
public:
    virtual ~Entity() = default;

    const std::string& GetName() const;
    const std::string& GetSpritePath() const;
    int GetLevel() const;

    int GetHp() const;
    int GetMaxHp() const;
    void SetHp(int value);
    int GetAttack() const;
    int GetDefense() const;
    int GetMagicAttack() const;
    int GetMagicDefense() const;

    int RollAttack() const;
    void ApplyDamage(int damage);
    void Heal(int value);
    bool IsAlive() const;

    const std::vector<combat::SkillDefinition>& GetSkills() const;

protected:
    Entity(std::string name, std::string spritePath, int level);

    void SetName(std::string value);
    void SetSpritePath(std::string value);
    void SetLevelInternal(int value);
    void SetStats(int maxHpValue, int attackValue, int defenseValue = 0, int magicAttackValue = 0, int magicDefenseValue = 0);
    void SetSkills(std::vector<combat::SkillDefinition> skills);

    std::string name;
    std::string spritePath;
    int level;
    int hp;
    int maxHp;
    int attack;
    int defense;
    int magicAttack;
    int magicDefense;
    std::vector<combat::SkillDefinition> availableSkills;
};
