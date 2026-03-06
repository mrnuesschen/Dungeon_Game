#include "entities/EnemyFactory.h"

#include "entities/Skeleton.h"
#include "entities/Slime.h"
#include "raylib.h"

std::unique_ptr<Enemy> CreateEnemy(EnemyArchetype archetype, int level) {
    const int clampedLevel = level < 1 ? 1 : level;

    switch (archetype) {
    case EnemyArchetype::SmallSlime:
        return std::make_unique<SmallSlimeEnemy>(clampedLevel);
    case EnemyArchetype::MediumSlime:
        return std::make_unique<MediumSlimeEnemy>(clampedLevel);
    case EnemyArchetype::LargeSlime:
        return std::make_unique<LargeSlimeEnemy>(clampedLevel);
    case EnemyArchetype::LesserSkeleton:
        return std::make_unique<LesserSkeletonEnemy>(clampedLevel);
    case EnemyArchetype::Skeleton:
        return std::make_unique<StandardSkeletonEnemy>(clampedLevel);
    case EnemyArchetype::SkeletonWarrior:
        return std::make_unique<SkeletonWarriorEnemy>(clampedLevel);
    }

    return std::make_unique<SmallSlimeEnemy>(clampedLevel);
}

EnemyArchetype RollRandomEnemyArchetype() {
    const int roll = GetRandomValue(0, 5);
    switch (roll) {
    case 0:
        return EnemyArchetype::SmallSlime;
    case 1:
        return EnemyArchetype::MediumSlime;
    case 2:
        return EnemyArchetype::LargeSlime;
    case 3:
        return EnemyArchetype::LesserSkeleton;
    case 4:
        return EnemyArchetype::Skeleton;
    case 5:
        return EnemyArchetype::SkeletonWarrior;
    default:
        return EnemyArchetype::SmallSlime;
    }
}
