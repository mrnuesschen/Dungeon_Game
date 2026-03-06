#pragma once

#include "entities/Enemy.h"

class SlimeEnemy : public Enemy {
public:
protected:
    explicit SlimeEnemy(EnemyArchetype archetype, std::string name, std::string spritePath, int level);
};

class SmallSlimeEnemy : public SlimeEnemy {
public:
    explicit SmallSlimeEnemy(int level);
};

class MediumSlimeEnemy : public SlimeEnemy {
public:
    explicit MediumSlimeEnemy(int level);
};

class LargeSlimeEnemy : public SlimeEnemy {
public:
    explicit LargeSlimeEnemy(int level);
};
