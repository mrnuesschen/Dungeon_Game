#include "entities/Skeleton.h"

SkeletonEnemy::SkeletonEnemy(SkeletonType type)
    : Enemy(
          type == SkeletonType::Lesser   ? EnemyArchetype::LesserSkeleton
          : type == SkeletonType::Standard ? EnemyArchetype::Skeleton
                                           : EnemyArchetype::SkeletonWarrior,
          type == SkeletonType::Lesser   ? "Lesser Skeleton"
          : type == SkeletonType::Standard ? "Skeleton"
                                           : "Skeleton Warrior",
          type == SkeletonType::Lesser   ? "assets/sprites/enemies/skeleton_level_0.png"
          : type == SkeletonType::Standard ? "assets/sprites/enemies/skeleton_level_20.png"
                                           : "assets/sprites/enemies/skeleton_level_40.png",
          type == SkeletonType::Lesser   ? 88
          : type == SkeletonType::Standard ? 124
                                           : 168,
          type == SkeletonType::Lesser   ? 10
          : type == SkeletonType::Standard ? 14
                                           : 18,
          type == SkeletonType::Lesser   ? 16
          : type == SkeletonType::Standard ? 22
                                           : 30) {}
