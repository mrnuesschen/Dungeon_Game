#pragma once

#include "entities/Enemy.h"

enum class SkeletonType {
    Lesser,
    Standard,
    Warrior
};

class SkeletonEnemy : public Enemy {
public:
    explicit SkeletonEnemy(SkeletonType type);
};
