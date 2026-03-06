#pragma once

#include <string>
#include <vector>

#include "GameTypes.h"
#include "entities/Entity.h"
#include "combat/SkillSystem.h"

class Enemy : public Entity {
public:
    virtual ~Enemy() = default;

    EnemyArchetype GetArchetype() const;
    int GetExpReward() const;

protected:
    Enemy(EnemyArchetype archetype, std::string name, std::string spritePath, int level, int expBaseReward);

    void ApplyScaling(int baseHp, int hpPerLevel, int baseAttack, int attackPerLevel, int baseDefense, int defensePerLevel);
    void SetBaseSkillsForArchetype(EnemyArchetype archetypeValue);
    void AddSkill(combat::SkillDefinition skill);

    EnemyArchetype archetype;
    int expBaseReward;
};
