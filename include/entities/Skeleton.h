#pragma once

#include "entities/Enemy.h"

class SkeletonEnemy : public Enemy {
public:
protected:
    explicit SkeletonEnemy(EnemyArchetype archetype, std::string name, std::string spritePath, int level);
};

class LesserSkeletonEnemy : public SkeletonEnemy {
public:
    explicit LesserSkeletonEnemy(int level);
};

class StandardSkeletonEnemy : public SkeletonEnemy {
public:
    explicit StandardSkeletonEnemy(int level);
};

class SkeletonWarriorEnemy : public SkeletonEnemy {
public:
    explicit SkeletonWarriorEnemy(int level);
};
