#pragma once

#include <string>

#include "GameTypes.h"

class Player {
public:
    Player();

    void SetClass(PlayerClass playerClass);
    PlayerClass GetClass() const;

    void SetCharacterName(const std::string& value);
    const std::string& GetCharacterName() const;

    void SetAvatarPath(const std::string& value);
    const std::string& GetAvatarPath() const;

    void ResetForBattle();

    int RollAttack() const;
    int RollHeal() const;

    void ApplyDamage(int damage);
    void Heal(int value);
    void SetHp(int value);

    bool IsAlive() const;
    bool IsDefending() const;
    void SetDefending(bool defending);

    int GetHp() const;
    int GetMaxHp() const;
    const std::string& GetClassName() const;

    static const char* ClassToString(PlayerClass playerClass);

private:
    void ApplyPreset();

    PlayerClass playerClass;
    std::string className;
    std::string characterName;
    std::string avatarPath;
    int hp;
    int maxHp;
    int attackMin;
    int attackMax;
    int healMin;
    int healMax;
    bool defending;
};
