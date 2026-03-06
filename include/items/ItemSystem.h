#pragma once

#include <string>
#include <vector>

namespace items {

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
    GuildSeal
};

struct ItemDefinition {
    ItemId id;
    const char* name;
    ItemCategory category;
    const char* description;
    int maxStack;
    int buyPrice;
};

const ItemDefinition& GetItemDefinition(ItemId id);
std::vector<ItemId> RollLootForEncounter(int floor);
const std::vector<ItemId>& GetShopItemIds();

} // namespace items
