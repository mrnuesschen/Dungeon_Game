#pragma once

#include <vector>

#include "combat/SkillSystem.h"

namespace combat {

enum class SkillId {
	ShieldBash,
	FortressStance,
	Shockwave,
	CrushingTaunt,

	Whirlwind,
	SunderStrike,
	ValiantGuard,
	LanceRush,

	PoisonedBlades,
	ShadowFlurry,
	VenomCloud,
	EvasionFocus,

	Firestorm,
	ArcaneBarrier,
	ChainLightning,
	FrostHex,

	PurifyingLight,
	WeakeningPrayer,
	Renew,
	SacredWave,

	AcidBomb,
	ToxicMist,
	VolatileFlask,
	CatalystSerum,

	JellyJab,
	StickySpit,
	SplitSplash,
	SludgeSlam,
	CorrosiveGoo,
	MireBurst,
	TitanBlob,
	AcidFlood,
	GelShield,
	BoneSlash,
	CripplingCut,
	RattleCharge,
	RustyCleave,
	GraveHex,
	BoneGuard,
	WarAxe,
	WarCry,
	ExecutionSpin
};

const SkillDefinition& GetSkillDefinition(SkillId id);
const std::vector<SkillDefinition>& GetAllSkillDefinitions();

}