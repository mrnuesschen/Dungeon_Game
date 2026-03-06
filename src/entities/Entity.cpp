#include "entities/Entity.h"

#include <algorithm>
#include <utility>

#include "raylib.h"

Entity::Entity(std::string nameValue, std::string spritePathValue, int levelValue)
    : name(std::move(nameValue)),
      spritePath(std::move(spritePathValue)),
      level(std::max(1, levelValue)),
      hp(1),
      maxHp(1),
            attack(1),
            defense(0),
            magicAttack(0),
            magicDefense(0),
      availableSkills() {}

const std::string& Entity::GetName() const {
    return name;
}

const std::string& Entity::GetSpritePath() const {
    return spritePath;
}

int Entity::GetLevel() const {
    return level;
}

int Entity::GetHp() const {
    return hp;
}

int Entity::GetMaxHp() const {
    return maxHp;
}

void Entity::SetHp(int value) {
    hp = std::clamp(value, 0, maxHp);
}

int Entity::GetAttack() const {
    return attack;
}

int Entity::GetDefense() const {
    return defense;
}

int Entity::GetMagicAttack() const {
    return magicAttack;
}

int Entity::GetMagicDefense() const {
    return magicDefense;
}

int Entity::RollAttack() const {
    return attack;
}

void Entity::ApplyDamage(int damage) {
    hp = std::max(0, hp - std::max(0, damage));
}

void Entity::Heal(int value) {
    hp = std::min(maxHp, hp + std::max(0, value));
}

bool Entity::IsAlive() const {
    return hp > 0;
}

const std::vector<combat::SkillDefinition>& Entity::GetSkills() const {
    return availableSkills;
}

void Entity::SetName(std::string value) {
    name = std::move(value);
}

void Entity::SetSpritePath(std::string value) {
    spritePath = std::move(value);
}

void Entity::SetLevelInternal(int value) {
    level = std::max(1, value);
}

void Entity::SetStats(int maxHpValue, int attackValue, int defenseValue, int magicAttackValue, int magicDefenseValue) {
    const int oldMaxHp = maxHp;
    maxHp = std::max(1, maxHpValue);
    attack = std::max(1, attackValue);
    defense = std::max(0, defenseValue);
    magicAttack = std::max(0, magicAttackValue);
    magicDefense = std::max(0, magicDefenseValue);

    if (oldMaxHp <= 0) {
        hp = maxHp;
        return;
    }

    const float hpRatio = static_cast<float>(hp) / static_cast<float>(oldMaxHp);
    hp = std::clamp(static_cast<int>(hpRatio * static_cast<float>(maxHp)), 0, maxHp);
}

void Entity::SetSkills(std::vector<combat::SkillDefinition> skills) {
    availableSkills = std::move(skills);
}
