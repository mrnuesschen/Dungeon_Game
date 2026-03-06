#pragma once

#include <string>
#include <vector>

#include "GameTypes.h"

namespace combat {

enum class SkillScalingStat {
    None,
    Attack,
    MagicAttack,
    Defense,
    MaxHp
};

enum class SkillEffectType {
    DirectDamage,
    AreaDamage,
    DamageOverTime,
    PlayerDamageReductionBuff,
    EnemyAttackDebuff,
    SelfHeal
};

struct SkillEffectDefinition {
    SkillEffectType type;
    int magnitude = 0;
    int durationTurns = 0;
    SkillScalingStat scalingStat = SkillScalingStat::None;
    int scalingPercent = 0;
};

struct SkillDefinition {
    std::string name;
    std::string description;
    std::vector<SkillEffectDefinition> effects;
    int manaCost = 0;
};

const std::vector<SkillDefinition>& GetSkillsForClass(PlayerClass playerClass);
const std::vector<SkillDefinition>& GetSkillsForEnemy(EnemyArchetype enemyType);

}