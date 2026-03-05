#pragma once

#include <string>
#include <vector>

#include "GameTypes.h"
#include "combat/SkillSystem.h"

class Enemy {
public:
    virtual ~Enemy() = default;

    EnemyArchetype GetArchetype() const;
    const std::string& GetName() const;
    const std::string& GetSpritePath() const;

    int GetHp() const;
    int GetMaxHp() const;
    void SetHp(int value);

    int RollAttack() const;
    void ApplyDamage(int damage);
    bool IsAlive() const;
    const std::vector<combat::SkillDefinition>& GetSkills() const;

protected:
    Enemy(EnemyArchetype archetype, std::string name, std::string spritePath, int maxHp, int attackMin, int attackMax);

    EnemyArchetype archetype;
    std::string name;
    std::string spritePath;
    int hp;
    int maxHp;
    int attackMin;
    int attackMax;
    std::vector<combat::SkillDefinition> availableSkills;
};
