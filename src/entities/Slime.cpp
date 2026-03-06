#include "entities/Slime.h"

#include <utility>

#include "combat/SkillDefinitions.h"

SlimeEnemy::SlimeEnemy(EnemyArchetype archetype, std::string name, std::string spritePath, int level)
    : Enemy(archetype, std::move(name), std::move(spritePath), level, 34) {
    ApplyScaling(70, 14, 8, 2, 14, 2);
}

SmallSlimeEnemy::SmallSlimeEnemy(int level)
    : SlimeEnemy(EnemyArchetype::SmallSlime, "Small Slime", "assets/sprites/enemies/slime_level_0.png", level) {}

MediumSlimeEnemy::MediumSlimeEnemy(int level)
    : SlimeEnemy(EnemyArchetype::MediumSlime, "Medium Slime", "assets/sprites/enemies/slime_level_20.png", level) {
    ApplyScaling(100, 17, 11, 2, 18, 2);
    AddSkill(combat::GetSkillDefinition(combat::SkillId::TitanBlob));
}

LargeSlimeEnemy::LargeSlimeEnemy(int level)
    : SlimeEnemy(EnemyArchetype::LargeSlime, "Large Slime", "assets/sprites/enemies/slime_level_40.png", level) {
    ApplyScaling(145, 22, 14, 2, 24, 3);
    AddSkill(combat::GetSkillDefinition(combat::SkillId::TitanBlob));
    AddSkill(combat::GetSkillDefinition(combat::SkillId::AcidFlood));
}
