#include "entities/Player.h"

#include <algorithm>

#include "raylib.h"

Player::Player()
    : playerClass(PlayerClass::Knight),
    className("Knight"),
    characterName("Hero"),
    avatarPath(""),
      hp(120),
      maxHp(120),
      attackMin(16),
      attackMax(28),
      healMin(10),
      healMax(18),
      defending(false) {
    ApplyPreset();
}

void Player::SetClass(PlayerClass value) {
    playerClass = value;
    ApplyPreset();
}

PlayerClass Player::GetClass() const {
    return playerClass;
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
    hp = maxHp;
    defending = false;
}

int Player::RollAttack() const {
    return GetRandomValue(attackMin, attackMax);
}

int Player::RollHeal() const {
    return GetRandomValue(healMin, healMax);
}

void Player::ApplyDamage(int damage) {
    hp = std::max(0, hp - std::max(0, damage));
}

void Player::Heal(int value) {
    hp = std::min(maxHp, hp + std::max(0, value));
}

void Player::SetHp(int value) {
    hp = std::clamp(value, 0, maxHp);
}

bool Player::IsAlive() const {
    return hp > 0;
}

bool Player::IsDefending() const {
    return defending;
}

void Player::SetDefending(bool value) {
    defending = value;
}

int Player::GetHp() const {
    return hp;
}

int Player::GetMaxHp() const {
    return maxHp;
}

const std::string& Player::GetClassName() const {
    return className;
}

const std::vector<combat::SkillDefinition>& Player::GetSkills() const {
    return availableSkills;
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
    availableSkills = combat::GetSkillsForClass(playerClass);

    switch (playerClass) {
    case PlayerClass::Tank:
        className = "Tank";
        maxHp = 170;
        attackMin = 12;
        attackMax = 20;
        healMin = 8;
        healMax = 14;
        break;
    case PlayerClass::Knight:
        className = "Knight";
        maxHp = 130;
        attackMin = 16;
        attackMax = 28;
        healMin = 10;
        healMax = 18;
        break;
    case PlayerClass::Assassin:
        className = "Assassin";
        maxHp = 98;
        attackMin = 22;
        attackMax = 36;
        healMin = 8;
        healMax = 14;
        break;
    case PlayerClass::Mage:
        className = "Mage";
        maxHp = 104;
        attackMin = 20;
        attackMax = 34;
        healMin = 12;
        healMax = 20;
        break;
    case PlayerClass::Healer:
        className = "Healer";
        maxHp = 118;
        attackMin = 12;
        attackMax = 22;
        healMin = 18;
        healMax = 30;
        break;
    case PlayerClass::Alchemist:
        className = "Alchemist";
        maxHp = 112;
        attackMin = 14;
        attackMax = 26;
        healMin = 14;
        healMax = 24;
        break;
    }

    if (characterName.empty()) {
        characterName = "Hero";
    }

    hp = std::min(hp, maxHp);
}
