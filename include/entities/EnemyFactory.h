#pragma once

#include <memory>

#include "GameTypes.h"
#include "entities/Enemy.h"

std::unique_ptr<Enemy> CreateEnemy(EnemyArchetype archetype, int level);
EnemyArchetype RollRandomEnemyArchetype();
