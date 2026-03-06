#include "items/ItemSystem.h"

#include <array>

#include "raylib.h"

namespace items {

namespace {

constexpr EquipSlotMask kNoEquipSlots = 0ULL;
constexpr EquipSlotMask kWeaponSlots = SlotMask(EquipmentSlot::Weapon1) | SlotMask(EquipmentSlot::Weapon2);
constexpr EquipSlotMask kHeadSlot = SlotMask(EquipmentSlot::Head);
constexpr EquipSlotMask kNecklaceSlot = SlotMask(EquipmentSlot::Necklace);
constexpr EquipSlotMask kHandSlots = SlotMask(EquipmentSlot::Hand1) | SlotMask(EquipmentSlot::Hand2);
constexpr EquipSlotMask kUpperInnerSlot = SlotMask(EquipmentSlot::UpperBodyInner);
constexpr EquipSlotMask kUpperOuterSlot = SlotMask(EquipmentSlot::UpperBodyOuter);
constexpr EquipSlotMask kUnderpantsSlot = SlotMask(EquipmentSlot::Underpants);
constexpr EquipSlotMask kPantsSlot = SlotMask(EquipmentSlot::Pants);
constexpr EquipSlotMask kShoesSlot = SlotMask(EquipmentSlot::Shoes);
constexpr EquipSlotMask kJewelrySlots = SlotMask(EquipmentSlot::Jewelry1) | SlotMask(EquipmentSlot::Jewelry2) |
                                        SlotMask(EquipmentSlot::Jewelry3) | SlotMask(EquipmentSlot::Jewelry4);
constexpr ItemDefinition::StatBonuses kNoBonuses{0, 0, 0, 0, 0};

} // namespace

bool IsValidItemIdValue(int rawValue) {
    return rawValue >= 0 && rawValue < static_cast<int>(ItemId::Count);
}

const ItemDefinition& GetItemDefinition(ItemId id) {
    static const ItemDefinition leatherCap{ItemId::LeatherCap, "Leather Cap", ItemCategory::Equipment, "Basic head protection for beginners.", 1, 24, kHeadSlot, ItemDefinition::StatBonuses{18, 0, 0, 0, 0}};
    static const ItemDefinition ironHelm{ItemId::IronHelm, "Iron Helm", ItemCategory::Equipment, "Sturdy helmet forged from iron.", 1, 42, kHeadSlot, ItemDefinition::StatBonuses{28, 1, 2, 0, 0}};
    static const ItemDefinition guardianCirclet{ItemId::GuardianCirclet, "Guardian Circlet", ItemCategory::Equipment, "Inscribed circlet that bolsters resilience.", 1, 58, kHeadSlot, ItemDefinition::StatBonuses{32, 2, 3, 0, 0}};
    static const ItemDefinition mysticHood{ItemId::MysticHood, "Mystic Hood", ItemCategory::Equipment, "A hood tuned to restorative magic.", 1, 74, kHeadSlot, ItemDefinition::StatBonuses{24, 0, 0, 3, 5}};
    static const ItemDefinition warCrown{ItemId::WarCrown, "War Crown", ItemCategory::Equipment, "Battle crown worn by veteran captains.", 1, 102, kHeadSlot, ItemDefinition::StatBonuses{45, 3, 5, 0, 0}};

    static const ItemDefinition silverPendant{ItemId::SilverPendant, "Silver Pendant", ItemCategory::Equipment, "A silver pendant with calming aura.", 1, 40, kNecklaceSlot, ItemDefinition::StatBonuses{12, 0, 0, 2, 3}};
    static const ItemDefinition runeNecklace{ItemId::RuneNecklace, "Rune Necklace", ItemCategory::Equipment, "Necklace etched with combat runes.", 1, 60, kNecklaceSlot, ItemDefinition::StatBonuses{0, 2, 4, 1, 2}};
    static const ItemDefinition guardianCharm{ItemId::GuardianCharm, "Guardian Charm", ItemCategory::Equipment, "Protective charm favored by defenders.", 1, 82, kNecklaceSlot, ItemDefinition::StatBonuses{26, 0, 1, 0, 1}};
    static const ItemDefinition dragonMedallion{ItemId::DragonMedallion, "Dragon Medallion", ItemCategory::Equipment, "A relic that amplifies all battle instincts.", 1, 118, kNecklaceSlot, ItemDefinition::StatBonuses{20, 3, 5, 3, 4}};

    static const ItemDefinition leatherGloves{ItemId::LeatherGloves, "Leather Gloves", ItemCategory::Equipment, "Simple gloves that improve grip.", 1, 26, kHandSlots, ItemDefinition::StatBonuses{0, 1, 2, 0, 0}};
    static const ItemDefinition ironGauntlets{ItemId::IronGauntlets, "Iron Gauntlets", ItemCategory::Equipment, "Heavy gauntlets for close combat.", 1, 48, kHandSlots, ItemDefinition::StatBonuses{20, 2, 3, 0, 0}};
    static const ItemDefinition duelistGloves{ItemId::DuelistGloves, "Duelist Gloves", ItemCategory::Equipment, "Balanced gloves for quick strikes.", 1, 62, kHandSlots, ItemDefinition::StatBonuses{0, 3, 4, 0, 0}};
    static const ItemDefinition mageGrips{ItemId::MageGrips, "Mage Grips", ItemCategory::Equipment, "Arcane grips that strengthen healing flow.", 1, 78, kHandSlots, ItemDefinition::StatBonuses{0, 0, 0, 3, 5}};
    static const ItemDefinition titanGauntlets{ItemId::TitanGauntlets, "Titan Gauntlets", ItemCategory::Equipment, "Massive gauntlets with crushing force.", 1, 110, kHandSlots, ItemDefinition::StatBonuses{24, 4, 6, 0, 0}};

    static const ItemDefinition linenShirt{ItemId::LinenShirt, "Linen Shirt", ItemCategory::Equipment, "Comfortable innerwear for long journeys.", 1, 20, kUpperInnerSlot, ItemDefinition::StatBonuses{14, 0, 0, 0, 1}};
    static const ItemDefinition paddedVest{ItemId::PaddedVest, "Padded Vest", ItemCategory::Equipment, "Quilted vest with reliable padding.", 1, 38, kUpperInnerSlot, ItemDefinition::StatBonuses{24, 1, 1, 0, 0}};
    static const ItemDefinition chainUndershirt{ItemId::ChainUndershirt, "Chain Undershirt", ItemCategory::Equipment, "Light chain layer beneath armor.", 1, 56, kUpperInnerSlot, ItemDefinition::StatBonuses{32, 1, 2, 0, 0}};
    static const ItemDefinition arcaneRobeInner{ItemId::ArcaneRobeInner, "Arcane Inner Robe", ItemCategory::Equipment, "Inner robe woven with mana threads.", 1, 76, kUpperInnerSlot, ItemDefinition::StatBonuses{18, 0, 0, 3, 4}};
    static const ItemDefinition dragonScaleLiner{ItemId::DragonScaleLiner, "Dragon Scale Liner", ItemCategory::Equipment, "Rare scaled liner with high protection.", 1, 104, kUpperInnerSlot, ItemDefinition::StatBonuses{44, 2, 3, 0, 0}};

    static const ItemDefinition knightCuirass{ItemId::KnightCuirass, "Knight Cuirass", ItemCategory::Equipment, "Standard cuirass of city guards.", 1, 64, kUpperOuterSlot, ItemDefinition::StatBonuses{36, 1, 2, 0, 0}};
    static const ItemDefinition shadowCoat{ItemId::ShadowCoat, "Shadow Coat", ItemCategory::Equipment, "Dark coat favored by scouts.", 1, 82, kUpperOuterSlot, ItemDefinition::StatBonuses{28, 2, 3, 0, 0}};
    static const ItemDefinition archmageMantle{ItemId::ArchmageMantle, "Archmage Mantle", ItemCategory::Equipment, "Enchanted mantle that empowers recovery.", 1, 102, kUpperOuterSlot, ItemDefinition::StatBonuses{26, 0, 1, 4, 5}};
    static const ItemDefinition titanPlate{ItemId::TitanPlate, "Titan Plate", ItemCategory::Equipment, "Heavy plate for frontline champions.", 1, 128, kUpperOuterSlot, ItemDefinition::StatBonuses{60, 2, 4, 0, 0}};

    static const ItemDefinition clothUnderwear{ItemId::ClothUnderwear, "Cloth Underwear", ItemCategory::Equipment, "Simple base layer.", 1, 8, kUnderpantsSlot, ItemDefinition::StatBonuses{8, 0, 0, 0, 0}};
    static const ItemDefinition travelerBriefs{ItemId::TravelerBriefs, "Traveler Briefs", ItemCategory::Equipment, "Comfortable briefs for expeditions.", 1, 16, kUnderpantsSlot, ItemDefinition::StatBonuses{12, 0, 0, 0, 1}};
    static const ItemDefinition guardLoincloth{ItemId::GuardLoincloth, "Guard Loincloth", ItemCategory::Equipment, "Guard-issued underlayer.", 1, 26, kUnderpantsSlot, ItemDefinition::StatBonuses{16, 1, 1, 0, 0}};
    static const ItemDefinition mysticUnderlayer{ItemId::MysticUnderlayer, "Mystic Underlayer", ItemCategory::Equipment, "Threaded with simple glyphs.", 1, 40, kUnderpantsSlot, ItemDefinition::StatBonuses{14, 0, 0, 2, 3}};
    static const ItemDefinition royalUnderwear{ItemId::RoyalUnderwear, "Royal Underwear", ItemCategory::Equipment, "Fine and surprisingly durable.", 1, 56, kUnderpantsSlot, ItemDefinition::StatBonuses{22, 1, 2, 1, 2}};

    static const ItemDefinition clothPants{ItemId::ClothPants, "Cloth Pants", ItemCategory::Equipment, "Loose pants with little protection.", 1, 18, kPantsSlot, ItemDefinition::StatBonuses{14, 0, 0, 0, 0}};
    static const ItemDefinition rangerLeggings{ItemId::RangerLeggings, "Ranger Leggings", ItemCategory::Equipment, "Light leggings for mobile fighters.", 1, 34, kPantsSlot, ItemDefinition::StatBonuses{20, 1, 2, 0, 0}};
    static const ItemDefinition ironGreaves{ItemId::IronGreaves, "Iron Greaves", ItemCategory::Equipment, "Reinforced leg armor.", 1, 54, kPantsSlot, ItemDefinition::StatBonuses{28, 1, 2, 0, 0}};
    static const ItemDefinition shadowTrousers{ItemId::ShadowTrousers, "Shadow Trousers", ItemCategory::Equipment, "Dark trousers tuned for precision.", 1, 72, kPantsSlot, ItemDefinition::StatBonuses{20, 2, 3, 0, 0}};
    static const ItemDefinition battleLegplates{ItemId::BattleLegplates, "Battle Legplates", ItemCategory::Equipment, "Heavy legplates for sustained combat.", 1, 98, kPantsSlot, ItemDefinition::StatBonuses{40, 2, 3, 0, 0}};

    static const ItemDefinition wornBoots{ItemId::WornBoots, "Worn Boots", ItemCategory::Equipment, "Old boots still fit for travel.", 1, 14, kShoesSlot, ItemDefinition::StatBonuses{10, 0, 1, 0, 0}};
    static const ItemDefinition leatherBoots{ItemId::LeatherBoots, "Leather Boots", ItemCategory::Equipment, "Reliable leather footwear.", 1, 28, kShoesSlot, ItemDefinition::StatBonuses{16, 1, 2, 0, 0}};
    static const ItemDefinition swiftBoots{ItemId::SwiftBoots, "Swift Boots", ItemCategory::Equipment, "Boots that help maintain rhythm in battle.", 1, 46, kShoesSlot, ItemDefinition::StatBonuses{16, 1, 3, 1, 1}};
    static const ItemDefinition ironSabatons{ItemId::IronSabatons, "Iron Sabatons", ItemCategory::Equipment, "Plate footwear for heavy armor users.", 1, 66, kShoesSlot, ItemDefinition::StatBonuses{26, 1, 2, 0, 0}};
    static const ItemDefinition phantomSteps{ItemId::PhantomSteps, "Phantom Steps", ItemCategory::Equipment, "Rare boots with refined balance.", 1, 92, kShoesSlot, ItemDefinition::StatBonuses{24, 2, 4, 1, 2}};

    static const ItemDefinition copperRing{ItemId::CopperRing, "Copper Ring", ItemCategory::Equipment, "Plain ring carrying small power.", 1, 22, kJewelrySlots, ItemDefinition::StatBonuses{8, 1, 1, 0, 1}};
    static const ItemDefinition silverRing{ItemId::SilverRing, "Silver Ring", ItemCategory::Equipment, "Polished ring with balanced bonuses.", 1, 40, kJewelrySlots, ItemDefinition::StatBonuses{12, 1, 2, 1, 2}};
    static const ItemDefinition emeraldRing{ItemId::EmeraldRing, "Emerald Ring", ItemCategory::Equipment, "Emerald inlay that aids healing.", 1, 60, kJewelrySlots, ItemDefinition::StatBonuses{10, 0, 1, 2, 4}};
    static const ItemDefinition sapphireRing{ItemId::SapphireRing, "Sapphire Ring", ItemCategory::Equipment, "Sapphire ring with offensive focus.", 1, 80, kJewelrySlots, ItemDefinition::StatBonuses{10, 2, 4, 0, 1}};
    static const ItemDefinition obsidianRing{ItemId::ObsidianRing, "Obsidian Ring", ItemCategory::Equipment, "Dark ring prized by veterans.", 1, 110, kJewelrySlots, ItemDefinition::StatBonuses{18, 3, 5, 1, 2}};

    static const ItemDefinition steelBlade{ItemId::SteelBlade, "Steel Blade", ItemCategory::Equipment, "A balanced steel blade for all-round fighters.", 1, 120, kWeaponSlots, ItemDefinition::StatBonuses{0, 11, 15, 0, 0}};
    static const ItemDefinition runeAxe{ItemId::RuneAxe, "Rune Axe", ItemCategory::Equipment, "Heavy axe marked with ancient symbols.", 1, 145, kWeaponSlots, ItemDefinition::StatBonuses{12, 13, 18, 0, 0}};
    static const ItemDefinition crystalSpear{ItemId::CrystalSpear, "Crystal Spear", ItemCategory::Equipment, "A sharp spear that channels precision.", 1, 170, kWeaponSlots, ItemDefinition::StatBonuses{0, 16, 22, 0, 0}};
    static const ItemDefinition bronzeSword{ItemId::BronzeSword, "Bronze Sword", ItemCategory::Equipment, "Simple weapon that increases attack potential.", 1, 45, kWeaponSlots, ItemDefinition::StatBonuses{0, 4, 6, 0, 0}};
    static const ItemDefinition ironSword{ItemId::IronSword, "Iron Sword", ItemCategory::Equipment, "Reliable sword forged for adventurers.", 1, 85, kWeaponSlots, ItemDefinition::StatBonuses{0, 8, 12, 0, 0}};
    static const ItemDefinition reinforcedArmor{ItemId::ReinforcedArmor, "Reinforced Armor", ItemCategory::Equipment, "Durable armor with better defense.", 1, 90, kUpperOuterSlot, ItemDefinition::StatBonuses{40, 0, 0, 0, 0}};
    static const ItemDefinition focusAmulet{ItemId::FocusAmulet, "Focus Amulet", ItemCategory::Equipment, "Charm that enhances battle focus.", 1, 70, kNecklaceSlot, ItemDefinition::StatBonuses{0, 0, 0, 4, 6}};
    static const ItemDefinition healthPotion{ItemId::HealthPotion, "Health Potion", ItemCategory::Consumable, "Restores some HP when consumed.", 20, 20, kNoEquipSlots, kNoBonuses};
    static const ItemDefinition greaterHealthPotion{ItemId::GreaterHealthPotion, "Greater Potion", ItemCategory::Consumable, "Restores a large amount of HP.", 20, 45, kNoEquipSlots, kNoBonuses};
    static const ItemDefinition elixir{ItemId::Elixir, "Elixir", ItemCategory::Consumable, "Rare brew used for difficult expeditions.", 10, 95, kNoEquipSlots, kNoBonuses};
    static const ItemDefinition ancientRelic{ItemId::AncientRelic, "Ancient Relic", ItemCategory::Quest, "A mysterious relic from deep dungeons.", 99, 0, kNoEquipSlots, kNoBonuses};
    static const ItemDefinition guildSeal{ItemId::GuildSeal, "Guild Seal", ItemCategory::Quest, "Proof of a completed guild objective.", 99, 0, kNoEquipSlots, kNoBonuses};

    switch (id) {
    case ItemId::LeatherCap:
        return leatherCap;
    case ItemId::IronHelm:
        return ironHelm;
    case ItemId::GuardianCirclet:
        return guardianCirclet;
    case ItemId::MysticHood:
        return mysticHood;
    case ItemId::WarCrown:
        return warCrown;
    case ItemId::SilverPendant:
        return silverPendant;
    case ItemId::RuneNecklace:
        return runeNecklace;
    case ItemId::GuardianCharm:
        return guardianCharm;
    case ItemId::DragonMedallion:
        return dragonMedallion;
    case ItemId::LeatherGloves:
        return leatherGloves;
    case ItemId::IronGauntlets:
        return ironGauntlets;
    case ItemId::DuelistGloves:
        return duelistGloves;
    case ItemId::MageGrips:
        return mageGrips;
    case ItemId::TitanGauntlets:
        return titanGauntlets;
    case ItemId::LinenShirt:
        return linenShirt;
    case ItemId::PaddedVest:
        return paddedVest;
    case ItemId::ChainUndershirt:
        return chainUndershirt;
    case ItemId::ArcaneRobeInner:
        return arcaneRobeInner;
    case ItemId::DragonScaleLiner:
        return dragonScaleLiner;
    case ItemId::KnightCuirass:
        return knightCuirass;
    case ItemId::ShadowCoat:
        return shadowCoat;
    case ItemId::ArchmageMantle:
        return archmageMantle;
    case ItemId::TitanPlate:
        return titanPlate;
    case ItemId::ClothUnderwear:
        return clothUnderwear;
    case ItemId::TravelerBriefs:
        return travelerBriefs;
    case ItemId::GuardLoincloth:
        return guardLoincloth;
    case ItemId::MysticUnderlayer:
        return mysticUnderlayer;
    case ItemId::RoyalUnderwear:
        return royalUnderwear;
    case ItemId::ClothPants:
        return clothPants;
    case ItemId::RangerLeggings:
        return rangerLeggings;
    case ItemId::IronGreaves:
        return ironGreaves;
    case ItemId::ShadowTrousers:
        return shadowTrousers;
    case ItemId::BattleLegplates:
        return battleLegplates;
    case ItemId::WornBoots:
        return wornBoots;
    case ItemId::LeatherBoots:
        return leatherBoots;
    case ItemId::SwiftBoots:
        return swiftBoots;
    case ItemId::IronSabatons:
        return ironSabatons;
    case ItemId::PhantomSteps:
        return phantomSteps;
    case ItemId::CopperRing:
        return copperRing;
    case ItemId::SilverRing:
        return silverRing;
    case ItemId::EmeraldRing:
        return emeraldRing;
    case ItemId::SapphireRing:
        return sapphireRing;
    case ItemId::ObsidianRing:
        return obsidianRing;
    case ItemId::SteelBlade:
        return steelBlade;
    case ItemId::RuneAxe:
        return runeAxe;
    case ItemId::CrystalSpear:
        return crystalSpear;
    case ItemId::BronzeSword:
        return bronzeSword;
    case ItemId::IronSword:
        return ironSword;
    case ItemId::ReinforcedArmor:
        return reinforcedArmor;
    case ItemId::FocusAmulet:
        return focusAmulet;
    case ItemId::HealthPotion:
        return healthPotion;
    case ItemId::GreaterHealthPotion:
        return greaterHealthPotion;
    case ItemId::Elixir:
        return elixir;
    case ItemId::AncientRelic:
        return ancientRelic;
    case ItemId::GuildSeal:
        return guildSeal;
    case ItemId::Count:
        break;
    }

    return healthPotion;
}

std::vector<ItemId> RollLootForEncounter(int floor) {
    std::vector<ItemId> loot;

    const int roll = GetRandomValue(0, 99);
    if (roll < 45) {
        loot.push_back(GetRandomValue(0, 100) < 75 ? ItemId::HealthPotion : ItemId::GreaterHealthPotion);
    } else if (roll < 82) {
        static const std::array<ItemId, 16> equipmentPool{
            ItemId::LeatherCap,
            ItemId::SilverPendant,
            ItemId::LeatherGloves,
            ItemId::LinenShirt,
            ItemId::ReinforcedArmor,
            ItemId::ClothUnderwear,
            ItemId::ClothPants,
            ItemId::WornBoots,
            ItemId::CopperRing,
            ItemId::BronzeSword,
            ItemId::IronSword,
            ItemId::SteelBlade,
            ItemId::RuneAxe,
            ItemId::CrystalSpear,
            ItemId::FocusAmulet,
            ItemId::TitanPlate};
        const int equipmentIndex = GetRandomValue(0, static_cast<int>(equipmentPool.size()) - 1);
        loot.push_back(equipmentPool[static_cast<size_t>(equipmentIndex)]);
    } else {
        static const std::array<ItemId, 2> questPool{
            ItemId::AncientRelic,
            ItemId::GuildSeal};
        const int questIndex = GetRandomValue(0, static_cast<int>(questPool.size()) - 1);
        loot.push_back(questPool[static_cast<size_t>(questIndex)]);
    }

    // Higher dungeon floors may yield an extra drop.
    if (floor >= 4 && GetRandomValue(0, 99) < 30) {
        const int bonusRoll = GetRandomValue(0, 2);
        if (bonusRoll == 0) {
            loot.push_back(ItemId::HealthPotion);
        } else if (bonusRoll == 1) {
            loot.push_back(ItemId::GreaterHealthPotion);
        } else {
            loot.push_back(ItemId::Elixir);
        }
    }

    return loot;
}

const std::vector<ItemId>& GetShopItemIds() {
    static const std::vector<ItemId> shopItems{
        ItemId::HealthPotion,
        ItemId::GreaterHealthPotion,
        ItemId::Elixir,
        ItemId::LeatherCap,
        ItemId::SilverPendant,
        ItemId::LeatherGloves,
        ItemId::LinenShirt,
        ItemId::ClothUnderwear,
        ItemId::ClothPants,
        ItemId::WornBoots,
        ItemId::CopperRing,
        ItemId::BronzeSword,
        ItemId::IronSword,
        ItemId::SteelBlade,
        ItemId::ReinforcedArmor,
        ItemId::FocusAmulet};

    return shopItems;
}

bool CanEquipInSlot(ItemId id, EquipmentSlot slot) {
    const ItemDefinition& definition = GetItemDefinition(id);
    return (definition.equipSlotMask & SlotMask(slot)) != 0ULL;
}

std::vector<EquipmentSlot> GetAllowedEquipmentSlots(ItemId id) {
    std::vector<EquipmentSlot> slots;
    for (int i = 0; i < static_cast<int>(EquipmentSlot::Count); ++i) {
        const EquipmentSlot slot = static_cast<EquipmentSlot>(i);
        if (CanEquipInSlot(id, slot)) {
            slots.push_back(slot);
        }
    }
    return slots;
}

const char* EquipmentSlotToString(EquipmentSlot slot) {
    switch (slot) {
    case EquipmentSlot::Head:
        return "Head";
    case EquipmentSlot::Necklace:
        return "Necklace";
    case EquipmentSlot::Hand1:
        return "Hand 1";
    case EquipmentSlot::Hand2:
        return "Hand 2";
    case EquipmentSlot::UpperBodyInner:
        return "Upper Body Inner";
    case EquipmentSlot::UpperBodyOuter:
        return "Upper Body Outer";
    case EquipmentSlot::Underpants:
        return "Underpants";
    case EquipmentSlot::Pants:
        return "Pants";
    case EquipmentSlot::Shoes:
        return "Shoes";
    case EquipmentSlot::Jewelry1:
        return "Jewelry 1";
    case EquipmentSlot::Jewelry2:
        return "Jewelry 2";
    case EquipmentSlot::Jewelry3:
        return "Jewelry 3";
    case EquipmentSlot::Jewelry4:
        return "Jewelry 4";
    case EquipmentSlot::Weapon1:
        return "Weapon 1";
    case EquipmentSlot::Weapon2:
        return "Weapon 2";
    case EquipmentSlot::Count:
        break;
    }

    return "Unknown";
}

} // namespace items
