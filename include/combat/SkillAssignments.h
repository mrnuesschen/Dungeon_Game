#pragma once

#include <vector>

#include "GameTypes.h"
#include "combat/SkillSystem.h"

namespace combat {

const std::vector<SkillDefinition>& GetClassSkillAssignments(PlayerClass playerClass);
const std::vector<SkillDefinition>& GetEnemySkillAssignments(EnemyArchetype enemyType);

}