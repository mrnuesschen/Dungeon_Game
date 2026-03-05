#include "entities/Enemy.h"

#include <algorithm>
#include <utility>

#include "raylib.h"

Enemy::Enemy(EnemyArchetype archetypeValue, std::string nameValue, std::string spritePathValue, int maxHpValue, int attackMinValue, int attackMaxValue)
    : archetype(archetypeValue),
      name(std::move(nameValue)),
            spritePath(std::move(spritePathValue)),
      hp(maxHpValue),
      maxHp(maxHpValue),
      attackMin(attackMinValue),
            attackMax(attackMaxValue),
            availableSkills(combat::GetSkillsForEnemy(archetypeValue)) {}

EnemyArchetype Enemy::GetArchetype() const {
    return archetype;
}

const std::string& Enemy::GetName() const {
    return name;
}

const std::string& Enemy::GetSpritePath() const {
    return spritePath;
}

int Enemy::GetHp() const {
    return hp;
}

int Enemy::GetMaxHp() const {
    return maxHp;
}

void Enemy::SetHp(int value) {
    hp = std::clamp(value, 0, maxHp);
}

int Enemy::RollAttack() const {
    return GetRandomValue(attackMin, attackMax);
}

void Enemy::ApplyDamage(int damage) {
    hp = std::max(0, hp - std::max(0, damage));
}

bool Enemy::IsAlive() const {
    return hp > 0;
}

const std::vector<combat::SkillDefinition>& Enemy::GetSkills() const {
    return availableSkills;
}
