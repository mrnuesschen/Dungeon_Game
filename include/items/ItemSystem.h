#pragma once

#include <string>
#include <vector>

namespace items {

enum class EquipmentSlot {
    Head,
    Necklace,
    Hand1,
    Hand2,
    UpperBodyInner,
    UpperBodyOuter,
    Underpants,
    Pants,
    Shoes,
    Jewelry1,
    Jewelry2,
    Jewelry3,
    Jewelry4,
    Weapon1,
    Weapon2,
    Count
};

using EquipSlotMask = unsigned long long;

constexpr EquipSlotMask SlotMask(EquipmentSlot slot) {
    return 1ULL << static_cast<int>(slot);
}

enum class ItemCategory {
    Equipment,
    Consumable,
    Quest
};

enum class ItemId {
    BronzeSword,
    IronSword,
    ReinforcedArmor,
    FocusAmulet,
    HealthPotion,
    GreaterHealthPotion,
    Elixir,
    AncientRelic,
    GuildSeal,
    LeatherCap,
    IronHelm,
    GuardianCirclet,
    MysticHood,
    WarCrown,
    SilverPendant,
    RuneNecklace,
    GuardianCharm,
    DragonMedallion,
    LeatherGloves,
    IronGauntlets,
    DuelistGloves,
    MageGrips,
    TitanGauntlets,
    LinenShirt,
    PaddedVest,
    ChainUndershirt,
    ArcaneRobeInner,
    DragonScaleLiner,
    KnightCuirass,
    ShadowCoat,
    ArchmageMantle,
    TitanPlate,
    ClothUnderwear,
    TravelerBriefs,
    GuardLoincloth,
    MysticUnderlayer,
    RoyalUnderwear,
    ClothPants,
    RangerLeggings,
    IronGreaves,
    ShadowTrousers,
    BattleLegplates,
    WornBoots,
    LeatherBoots,
    SwiftBoots,
    IronSabatons,
    PhantomSteps,
    CopperRing,
    SilverRing,
    EmeraldRing,
    SapphireRing,
    ObsidianRing,
    SteelBlade,
    RuneAxe,
    CrystalSpear,
    Count
};

struct ItemDefinition {
    struct StatBonuses {
        int maxHp = 0;
        int attackMin = 0;
        int attackMax = 0;
        int defense = 0;
        int healMin = 0;
        int healMax = 0;
        int maxMana = 0;
        int magicAttack = 0;
        int magicDefense = 0;
        int hpRegen = 0;
        int manaRegen = 0;
    };

    ItemId id;
    const char* name;
    ItemCategory category;
    const char* description;
    int maxStack;
    int buyPrice;
    EquipSlotMask equipSlotMask;
    StatBonuses statBonuses;
};

const ItemDefinition& GetItemDefinition(ItemId id);
bool IsValidItemIdValue(int rawValue);
std::vector<ItemId> RollLootForEncounter(int floor);
const std::vector<ItemId>& GetShopItemIds();
bool CanEquipInSlot(ItemId id, EquipmentSlot slot);
std::vector<EquipmentSlot> GetAllowedEquipmentSlots(ItemId id);
const char* EquipmentSlotToString(EquipmentSlot slot);

} // namespace items
