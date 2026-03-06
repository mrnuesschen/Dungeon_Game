#include "entities/Skeleton.h"

#include <utility>

#include "combat/SkillDefinitions.h"

SkeletonEnemy::SkeletonEnemy(EnemyArchetype archetype, std::string name, std::string spritePath, int level)
    : Enemy(archetype, std::move(name), std::move(spritePath), level, 46) {
    ApplyScaling(88, 16, 13, 2, 3, 1);
}

LesserSkeletonEnemy::LesserSkeletonEnemy(int level)
    : SkeletonEnemy(EnemyArchetype::LesserSkeleton, "Lesser Skeleton", "assets/sprites/enemies/skeleton_level_0.png", level) {}

StandardSkeletonEnemy::StandardSkeletonEnemy(int level)
    : SkeletonEnemy(EnemyArchetype::Skeleton, "Skeleton", "assets/sprites/enemies/skeleton_level_20.png", level) {
    ApplyScaling(124, 20, 18, 3, 5, 1);
    AddSkill(combat::GetSkillDefinition(combat::SkillId::WarCry));
}

SkeletonWarriorEnemy::SkeletonWarriorEnemy(int level)
    : SkeletonEnemy(EnemyArchetype::SkeletonWarrior, "Skeleton Warrior", "assets/sprites/enemies/skeleton_level_40.png", level) {
    ApplyScaling(168, 25, 24, 3, 8, 2);
    AddSkill(combat::GetSkillDefinition(combat::SkillId::WarCry));
    AddSkill(combat::GetSkillDefinition(combat::SkillId::ExecutionSpin));
}


                                           

