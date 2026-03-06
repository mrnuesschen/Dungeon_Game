#include "entities/Enemy.h"

#include <algorithm>
#include <utility>

Enemy::Enemy(EnemyArchetype archetypeValue, std::string nameValue, std::string spritePathValue, int levelValue, int expBaseRewardValue)
    : Entity(std::move(nameValue), std::move(spritePathValue), levelValue),
      archetype(archetypeValue),
      expBaseReward(std::max(1, expBaseRewardValue)) {
    SetBaseSkillsForArchetype(archetypeValue);
}

EnemyArchetype Enemy::GetArchetype() const {
    return archetype;
}

int Enemy::GetExpReward() const {
    return expBaseReward + (GetLevel() - 1) * 12;
}

void Enemy::ApplyScaling(int baseHp, int hpPerLevel, int baseAttackMin, int attackMinPerLevel, int baseAttackMax, int attackMaxPerLevel) {
    const int levelOffset = GetLevel() - 1;
    const int scaledMaxHp = baseHp + hpPerLevel * levelOffset;
    const int scaledAttackMin = baseAttackMin + attackMinPerLevel * levelOffset;
    const int scaledAttackMax = baseAttackMax + attackMaxPerLevel * levelOffset;

    SetStats(scaledMaxHp, scaledAttackMin, scaledAttackMax);
    SetHp(GetMaxHp());
}

void Enemy::SetBaseSkillsForArchetype(EnemyArchetype archetypeValue) {
    const auto& baseSkills = combat::GetSkillsForEnemy(archetypeValue);
    SetSkills(std::vector<combat::SkillDefinition>(baseSkills.begin(), baseSkills.end()));
}

void Enemy::AddSkill(combat::SkillDefinition skill) {
    auto skills = GetSkills();
    skills.push_back(std::move(skill));
    SetSkills(std::move(skills));
}
