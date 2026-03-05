#pragma once

#include "entities/Enemy.h"

enum class SlimeType {
    Small,
    Medium,
    Large
};

class SlimeEnemy : public Enemy {
public:
    explicit SlimeEnemy(SlimeType type);
};
