#include "entities/EnemyFactory.h"

#include "entities/Skeleton.h"
#include "entities/Slime.h"
#include "raylib.h"

std::unique_ptr<Enemy> CreateEnemy(EnemyArchetype archetype) {
    switch (archetype) {
    case EnemyArchetype::SmallSlime:
        return std::make_unique<SlimeEnemy>(SlimeType::Small);
    case EnemyArchetype::MediumSlime:
        return std::make_unique<SlimeEnemy>(SlimeType::Medium);
    case EnemyArchetype::LargeSlime:
        return std::make_unique<SlimeEnemy>(SlimeType::Large);
    case EnemyArchetype::LesserSkeleton:
        return std::make_unique<SkeletonEnemy>(SkeletonType::Lesser);
    case EnemyArchetype::Skeleton:
        return std::make_unique<SkeletonEnemy>(SkeletonType::Standard);
    case EnemyArchetype::SkeletonWarrior:
        return std::make_unique<SkeletonEnemy>(SkeletonType::Warrior);
    }

    return std::make_unique<SlimeEnemy>(SlimeType::Small);
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
