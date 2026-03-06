#include "items/ItemSystem.h"

#include <array>

#include "raylib.h"

namespace items {

const ItemDefinition& GetItemDefinition(ItemId id) {
    static const ItemDefinition bronzeSword{ItemId::BronzeSword, "Bronze Sword", ItemCategory::Equipment, "Simple weapon that increases attack potential.", 1, 45};
    static const ItemDefinition ironSword{ItemId::IronSword, "Iron Sword", ItemCategory::Equipment, "Reliable sword forged for adventurers.", 1, 85};
    static const ItemDefinition reinforcedArmor{ItemId::ReinforcedArmor, "Reinforced Armor", ItemCategory::Equipment, "Durable armor with better defense.", 1, 90};
    static const ItemDefinition focusAmulet{ItemId::FocusAmulet, "Focus Amulet", ItemCategory::Equipment, "Charm that enhances battle focus.", 1, 70};
    static const ItemDefinition healthPotion{ItemId::HealthPotion, "Health Potion", ItemCategory::Consumable, "Restores some HP when consumed.", 20, 20};
    static const ItemDefinition greaterHealthPotion{ItemId::GreaterHealthPotion, "Greater Potion", ItemCategory::Consumable, "Restores a large amount of HP.", 20, 45};
    static const ItemDefinition elixir{ItemId::Elixir, "Elixir", ItemCategory::Consumable, "Rare brew used for difficult expeditions.", 10, 95};
    static const ItemDefinition ancientRelic{ItemId::AncientRelic, "Ancient Relic", ItemCategory::Quest, "A mysterious relic from deep dungeons.", 99, 0};
    static const ItemDefinition guildSeal{ItemId::GuildSeal, "Guild Seal", ItemCategory::Quest, "Proof of a completed guild objective.", 99, 0};

    switch (id) {
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
    }

    return healthPotion;
}

std::vector<ItemId> RollLootForEncounter(int floor) {
    std::vector<ItemId> loot;

    const int roll = GetRandomValue(0, 99);
    if (roll < 45) {
        loot.push_back(GetRandomValue(0, 100) < 75 ? ItemId::HealthPotion : ItemId::GreaterHealthPotion);
    } else if (roll < 82) {
        static const std::array<ItemId, 4> equipmentPool{
            ItemId::BronzeSword,
            ItemId::IronSword,
            ItemId::ReinforcedArmor,
            ItemId::FocusAmulet};
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
        ItemId::BronzeSword,
        ItemId::IronSword,
        ItemId::ReinforcedArmor,
        ItemId::FocusAmulet};

    return shopItems;
}

} // namespace items
