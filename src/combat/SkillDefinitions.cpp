#include "combat/SkillDefinitions.h"

#include <stdexcept>

namespace combat {

const SkillDefinition& GetSkillDefinition(SkillId id) {
    static const SkillDefinition shieldBash{"Shield Bash", "Melee hit and weakens one enemy attack for 2 turns.", {{SkillEffectType::DirectDamage, 14, 0}, {SkillEffectType::EnemyAttackDebuff, 4, 2}}, 8};
    static const SkillDefinition fortressStance{"Fortress Stance", "Gain heavy damage reduction for 2 turns.", {{SkillEffectType::PlayerDamageReductionBuff, 6, 2}}, 10};
    static const SkillDefinition shockwave{"Shockwave", "Ground slam hitting all enemies.", {{SkillEffectType::AreaDamage, 10, 0}}, 12};
    static const SkillDefinition crushingTaunt{"Crushing Taunt", "Strike one enemy and apply a short attack debuff.", {{SkillEffectType::DirectDamage, 11, 0}, {SkillEffectType::EnemyAttackDebuff, 3, 1}}, 9};

    static const SkillDefinition whirlwind{"Whirlwind", "Spin attack that hits all enemies.", {{SkillEffectType::AreaDamage, 15, 0}}, 12};
    static const SkillDefinition sunderStrike{"Sunder Strike", "Heavy strike and bleeding over time.", {{SkillEffectType::DirectDamage, 12, 0}, {SkillEffectType::DamageOverTime, 7, 2}}, 11};
    static const SkillDefinition valiantGuard{"Valiant Guard", "Block incoming damage for 2 turns.", {{SkillEffectType::PlayerDamageReductionBuff, 4, 2}}, 9};
    static const SkillDefinition lanceRush{"Lance Rush", "Piercing hit with additional chip damage over time.", {{SkillEffectType::DirectDamage, 16, 0}, {SkillEffectType::DamageOverTime, 4, 2}}, 13};

    static const SkillDefinition poisonedBlades{"Poisoned Blades", "Single-target strike with poison over time.", {{SkillEffectType::DirectDamage, 10, 0}, {SkillEffectType::DamageOverTime, 9, 3}}, 10};
    static const SkillDefinition shadowFlurry{"Shadow Flurry", "Fast area assault that weakens enemies.", {{SkillEffectType::AreaDamage, 12, 0}, {SkillEffectType::EnemyAttackDebuff, 3, 1}}, 12};
    static const SkillDefinition venomCloud{"Venom Cloud", "Poisons all enemies for multiple turns.", {{SkillEffectType::DamageOverTime, 7, 3}}, 11};
    static const SkillDefinition evasionFocus{"Evasion Focus", "Reduce incoming damage while attacking.", {{SkillEffectType::PlayerDamageReductionBuff, 4, 2}, {SkillEffectType::DirectDamage, 8, 0}}, 9};

    static const SkillDefinition firestorm{"Firestorm", "Fire burst on all enemies plus burn over time.", {{SkillEffectType::AreaDamage, 13, 0}, {SkillEffectType::DamageOverTime, 6, 2}}, 14};
    static const SkillDefinition arcaneBarrier{"Arcane Barrier", "Magic shield and a focused bolt.", {{SkillEffectType::PlayerDamageReductionBuff, 5, 2}, {SkillEffectType::DirectDamage, 11, 0}}, 12};
    static const SkillDefinition chainLightning{"Chain Lightning", "High area burst damage.", {{SkillEffectType::AreaDamage, 16, 0}}, 16};
    static const SkillDefinition frostHex{"Frost Hex", "Single spell that lowers enemy attack for 2 turns.", {{SkillEffectType::DirectDamage, 9, 0}, {SkillEffectType::EnemyAttackDebuff, 5, 2}}, 11};

    static const SkillDefinition purifyingLight{"Purifying Light", "Heal yourself and smite one enemy.", {{SkillEffectType::SelfHeal, 20, 0}, {SkillEffectType::DirectDamage, 9, 0}}, 13};
    static const SkillDefinition weakeningPrayer{"Weakening Prayer", "Soft area damage and enemy attack debuff.", {{SkillEffectType::AreaDamage, 8, 0}, {SkillEffectType::EnemyAttackDebuff, 4, 2}}, 11};
    static const SkillDefinition renew{"Renew", "Strong self-heal and small defense buff.", {{SkillEffectType::SelfHeal, 24, 0}, {SkillEffectType::PlayerDamageReductionBuff, 3, 1}}, 14};
    static const SkillDefinition sacredWave{"Sacred Wave", "Holy wave against all enemies.", {{SkillEffectType::AreaDamage, 12, 0}}, 12};

    static const SkillDefinition acidBomb{"Acid Bomb", "Area explosion with acid damage over time.", {{SkillEffectType::AreaDamage, 11, 0}, {SkillEffectType::DamageOverTime, 8, 2}}, 13};
    static const SkillDefinition toxicMist{"Toxic Mist", "Poison cloud that lowers enemy attack.", {{SkillEffectType::DamageOverTime, 7, 2}, {SkillEffectType::EnemyAttackDebuff, 4, 2}}, 12};
    static const SkillDefinition volatileFlask{"Volatile Flask", "Strong direct blast to one enemy.", {{SkillEffectType::DirectDamage, 18, 0}}, 14};
    static const SkillDefinition catalystSerum{"Catalyst Serum", "Enhance survivability and recover health.", {{SkillEffectType::PlayerDamageReductionBuff, 5, 2}, {SkillEffectType::SelfHeal, 14, 0}}, 11};

    static const SkillDefinition jellyJab{"Jelly Jab", "Quick hit against one target.", {{SkillEffectType::DirectDamage, 8, 0}}};
    static const SkillDefinition stickySpit{"Sticky Spit", "Weak hit that reduces player damage for 1 turn.", {{SkillEffectType::DirectDamage, 5, 0}, {SkillEffectType::EnemyAttackDebuff, 2, 1}}};
    static const SkillDefinition splitSplash{"Split Splash", "Small area splash hit.", {{SkillEffectType::AreaDamage, 4, 0}}};

    static const SkillDefinition sludgeSlam{"Sludge Slam", "Heavy slime strike.", {{SkillEffectType::DirectDamage, 12, 0}}};
    static const SkillDefinition corrosiveGoo{"Corrosive Goo", "Applies acid damage over time.", {{SkillEffectType::DamageOverTime, 6, 2}}};
    static const SkillDefinition mireBurst{"Mire Burst", "Slime burst against all foes.", {{SkillEffectType::AreaDamage, 7, 0}}};

    static const SkillDefinition titanBlob{"Titan Blob", "Massive body slam.", {{SkillEffectType::DirectDamage, 16, 0}}};
    static const SkillDefinition acidFlood{"Acid Flood", "Area corrosion with damage over time.", {{SkillEffectType::AreaDamage, 9, 0}, {SkillEffectType::DamageOverTime, 5, 2}}};
    static const SkillDefinition gelShield{"Gel Shield", "Defensive slime layer reduces incoming damage.", {{SkillEffectType::PlayerDamageReductionBuff, 4, 2}}};

    static const SkillDefinition boneSlash{"Bone Slash", "Simple weapon strike.", {{SkillEffectType::DirectDamage, 9, 0}}};
    static const SkillDefinition cripplingCut{"Crippling Cut", "Cut that lowers attack briefly.", {{SkillEffectType::DirectDamage, 7, 0}, {SkillEffectType::EnemyAttackDebuff, 3, 1}}};
    static const SkillDefinition rattleCharge{"Rattle Charge", "Bone rush against multiple targets.", {{SkillEffectType::AreaDamage, 6, 0}}};

    static const SkillDefinition rustyCleave{"Rusty Cleave", "Solid melee hit.", {{SkillEffectType::DirectDamage, 12, 0}}};
    static const SkillDefinition graveHex{"Grave Hex", "Cursed wound causing damage over time.", {{SkillEffectType::DirectDamage, 8, 0}, {SkillEffectType::DamageOverTime, 6, 2}}};
    static const SkillDefinition boneGuard{"Bone Guard", "Bone armor for temporary damage reduction.", {{SkillEffectType::PlayerDamageReductionBuff, 3, 2}}};

    static const SkillDefinition warAxe{"War Axe", "Powerful single-target blow.", {{SkillEffectType::DirectDamage, 17, 0}}};
    static const SkillDefinition warCry{"War Cry", "Battle shout that weakens enemy offense.", {{SkillEffectType::EnemyAttackDebuff, 5, 2}}};
    static const SkillDefinition executionSpin{"Execution Spin", "Heavy spinning attack on all targets.", {{SkillEffectType::AreaDamage, 10, 0}}};

    switch (id) {
    case SkillId::ShieldBash:
        return shieldBash;
    case SkillId::FortressStance:
        return fortressStance;
    case SkillId::Shockwave:
        return shockwave;
    case SkillId::CrushingTaunt:
        return crushingTaunt;
    case SkillId::Whirlwind:
        return whirlwind;
    case SkillId::SunderStrike:
        return sunderStrike;
    case SkillId::ValiantGuard:
        return valiantGuard;
    case SkillId::LanceRush:
        return lanceRush;
    case SkillId::PoisonedBlades:
        return poisonedBlades;
    case SkillId::ShadowFlurry:
        return shadowFlurry;
    case SkillId::VenomCloud:
        return venomCloud;
    case SkillId::EvasionFocus:
        return evasionFocus;
    case SkillId::Firestorm:
        return firestorm;
    case SkillId::ArcaneBarrier:
        return arcaneBarrier;
    case SkillId::ChainLightning:
        return chainLightning;
    case SkillId::FrostHex:
        return frostHex;
    case SkillId::PurifyingLight:
        return purifyingLight;
    case SkillId::WeakeningPrayer:
        return weakeningPrayer;
    case SkillId::Renew:
        return renew;
    case SkillId::SacredWave:
        return sacredWave;
    case SkillId::AcidBomb:
        return acidBomb;
    case SkillId::ToxicMist:
        return toxicMist;
    case SkillId::VolatileFlask:
        return volatileFlask;
    case SkillId::CatalystSerum:
        return catalystSerum;
    case SkillId::JellyJab:
        return jellyJab;
    case SkillId::StickySpit:
        return stickySpit;
    case SkillId::SplitSplash:
        return splitSplash;
    case SkillId::SludgeSlam:
        return sludgeSlam;
    case SkillId::CorrosiveGoo:
        return corrosiveGoo;
    case SkillId::MireBurst:
        return mireBurst;
    case SkillId::TitanBlob:
        return titanBlob;
    case SkillId::AcidFlood:
        return acidFlood;
    case SkillId::GelShield:
        return gelShield;
    case SkillId::BoneSlash:
        return boneSlash;
    case SkillId::CripplingCut:
        return cripplingCut;
    case SkillId::RattleCharge:
        return rattleCharge;
    case SkillId::RustyCleave:
        return rustyCleave;
    case SkillId::GraveHex:
        return graveHex;
    case SkillId::BoneGuard:
        return boneGuard;
    case SkillId::WarAxe:
        return warAxe;
    case SkillId::WarCry:
        return warCry;
    case SkillId::ExecutionSpin:
        return executionSpin;
    }

    throw std::invalid_argument("Unknown skill id");
}

const std::vector<SkillDefinition>& GetAllSkillDefinitions() {
    static const std::vector<SkillDefinition> allSkills{
        GetSkillDefinition(SkillId::ShieldBash),
        GetSkillDefinition(SkillId::FortressStance),
        GetSkillDefinition(SkillId::Shockwave),
        GetSkillDefinition(SkillId::CrushingTaunt),
        GetSkillDefinition(SkillId::Whirlwind),
        GetSkillDefinition(SkillId::SunderStrike),
        GetSkillDefinition(SkillId::ValiantGuard),
        GetSkillDefinition(SkillId::LanceRush),
        GetSkillDefinition(SkillId::PoisonedBlades),
        GetSkillDefinition(SkillId::ShadowFlurry),
        GetSkillDefinition(SkillId::VenomCloud),
        GetSkillDefinition(SkillId::EvasionFocus),
        GetSkillDefinition(SkillId::Firestorm),
        GetSkillDefinition(SkillId::ArcaneBarrier),
        GetSkillDefinition(SkillId::ChainLightning),
        GetSkillDefinition(SkillId::FrostHex),
        GetSkillDefinition(SkillId::PurifyingLight),
        GetSkillDefinition(SkillId::WeakeningPrayer),
        GetSkillDefinition(SkillId::Renew),
        GetSkillDefinition(SkillId::SacredWave),
        GetSkillDefinition(SkillId::AcidBomb),
        GetSkillDefinition(SkillId::ToxicMist),
        GetSkillDefinition(SkillId::VolatileFlask),
        GetSkillDefinition(SkillId::CatalystSerum),
        GetSkillDefinition(SkillId::JellyJab),
        GetSkillDefinition(SkillId::StickySpit),
        GetSkillDefinition(SkillId::SplitSplash),
        GetSkillDefinition(SkillId::SludgeSlam),
        GetSkillDefinition(SkillId::CorrosiveGoo),
        GetSkillDefinition(SkillId::MireBurst),
        GetSkillDefinition(SkillId::TitanBlob),
        GetSkillDefinition(SkillId::AcidFlood),
        GetSkillDefinition(SkillId::GelShield),
        GetSkillDefinition(SkillId::BoneSlash),
        GetSkillDefinition(SkillId::CripplingCut),
        GetSkillDefinition(SkillId::RattleCharge),
        GetSkillDefinition(SkillId::RustyCleave),
        GetSkillDefinition(SkillId::GraveHex),
        GetSkillDefinition(SkillId::BoneGuard),
        GetSkillDefinition(SkillId::WarAxe),
        GetSkillDefinition(SkillId::WarCry),
        GetSkillDefinition(SkillId::ExecutionSpin)};

    return allSkills;
}

}