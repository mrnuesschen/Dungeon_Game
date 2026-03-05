#include "entities/Slime.h"

SlimeEnemy::SlimeEnemy(SlimeType type)
    : Enemy(
          type == SlimeType::Small   ? EnemyArchetype::SmallSlime
          : type == SlimeType::Medium ? EnemyArchetype::MediumSlime
                                      : EnemyArchetype::LargeSlime,
          type == SlimeType::Small   ? "Small Slime"
          : type == SlimeType::Medium ? "Medium Slime"
                                      : "Large Slime",
          type == SlimeType::Small   ? "assets/sprites/enemies/slime_level_0.png"
          : type == SlimeType::Medium ? "assets/sprites/enemies/slime_level_20.png"
                                      : "assets/sprites/enemies/slime_level_40.png",
          type == SlimeType::Small   ? 70
          : type == SlimeType::Medium ? 100
                                      : 145,
          type == SlimeType::Small   ? 8
          : type == SlimeType::Medium ? 11
                                      : 14,
          type == SlimeType::Small   ? 14
          : type == SlimeType::Medium ? 18
                                      : 24) {}
