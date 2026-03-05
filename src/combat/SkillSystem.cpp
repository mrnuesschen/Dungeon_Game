#include "combat/SkillSystem.h"

#include "combat/SkillAssignments.h"

namespace combat {

const std::vector<SkillDefinition>& GetSkillsForClass(PlayerClass playerClass) {
    return GetClassSkillAssignments(playerClass);
}

const std::vector<SkillDefinition>& GetSkillsForEnemy(EnemyArchetype enemyType) {
    return GetEnemySkillAssignments(enemyType);
}

}