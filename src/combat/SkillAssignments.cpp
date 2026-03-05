#include "combat/SkillAssignments.h"

#include <initializer_list>

#include "combat/SkillDefinitions.h"

namespace combat {

namespace {

std::vector<SkillDefinition> MakeSkillSet(std::initializer_list<SkillId> ids) {
    std::vector<SkillDefinition> result;
    result.reserve(ids.size());
    for (const SkillId id : ids) {
        result.push_back(GetSkillDefinition(id));
    }
    return result;
}

} // namespace

const std::vector<SkillDefinition>& GetClassSkillAssignments(PlayerClass playerClass) {
    static const std::vector<SkillDefinition> tankSkills = MakeSkillSet({SkillId::ShieldBash, SkillId::FortressStance, SkillId::Shockwave, SkillId::CrushingTaunt});
    static const std::vector<SkillDefinition> knightSkills = MakeSkillSet({SkillId::Whirlwind, SkillId::SunderStrike, SkillId::ValiantGuard, SkillId::LanceRush});
    static const std::vector<SkillDefinition> assassinSkills = MakeSkillSet({SkillId::PoisonedBlades, SkillId::ShadowFlurry, SkillId::VenomCloud, SkillId::EvasionFocus});
    static const std::vector<SkillDefinition> mageSkills = MakeSkillSet({SkillId::Firestorm, SkillId::ArcaneBarrier, SkillId::ChainLightning, SkillId::FrostHex});
    static const std::vector<SkillDefinition> healerSkills = MakeSkillSet({SkillId::PurifyingLight, SkillId::WeakeningPrayer, SkillId::Renew, SkillId::SacredWave});
    static const std::vector<SkillDefinition> alchemistSkills = MakeSkillSet({SkillId::AcidBomb, SkillId::ToxicMist, SkillId::VolatileFlask, SkillId::CatalystSerum});

    switch (playerClass) {
    case PlayerClass::Tank:
        return tankSkills;
    case PlayerClass::Knight:
        return knightSkills;
    case PlayerClass::Assassin:
        return assassinSkills;
    case PlayerClass::Mage:
        return mageSkills;
    case PlayerClass::Healer:
        return healerSkills;
    case PlayerClass::Alchemist:
        return alchemistSkills;
    }

    return knightSkills;
}

const std::vector<SkillDefinition>& GetEnemySkillAssignments(EnemyArchetype enemyType) {
    static const std::vector<SkillDefinition> smallSlimeSkills = MakeSkillSet({SkillId::JellyJab, SkillId::StickySpit, SkillId::SplitSplash});
    static const std::vector<SkillDefinition> mediumSlimeSkills = MakeSkillSet({SkillId::SludgeSlam, SkillId::CorrosiveGoo, SkillId::MireBurst});
    static const std::vector<SkillDefinition> largeSlimeSkills = MakeSkillSet({SkillId::TitanBlob, SkillId::AcidFlood, SkillId::GelShield});
    static const std::vector<SkillDefinition> lesserSkeletonSkills = MakeSkillSet({SkillId::BoneSlash, SkillId::CripplingCut, SkillId::RattleCharge});
    static const std::vector<SkillDefinition> skeletonSkills = MakeSkillSet({SkillId::RustyCleave, SkillId::GraveHex, SkillId::BoneGuard});
    static const std::vector<SkillDefinition> skeletonWarriorSkills = MakeSkillSet({SkillId::WarAxe, SkillId::WarCry, SkillId::ExecutionSpin});

    switch (enemyType) {
    case EnemyArchetype::SmallSlime:
        return smallSlimeSkills;
    case EnemyArchetype::MediumSlime:
        return mediumSlimeSkills;
    case EnemyArchetype::LargeSlime:
        return largeSlimeSkills;
    case EnemyArchetype::LesserSkeleton:
        return lesserSkeletonSkills;
    case EnemyArchetype::Skeleton:
        return skeletonSkills;
    case EnemyArchetype::SkeletonWarrior:
        return skeletonWarriorSkills;
    }

    return smallSlimeSkills;
}

}