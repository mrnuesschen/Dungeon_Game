#pragma once

#include <vector>

#include "items/ItemSystem.h"

namespace items {

struct InventoryEntry {
    ItemId id;
    int quantity;
};

class Inventory {
public:
    bool CanAddItem(ItemId id, int quantity = 1) const;
    void Clear();
    void SetEntries(const std::vector<InventoryEntry>& value);
    void AddItem(ItemId id, int quantity = 1);
    bool RemoveItem(ItemId id, int quantity = 1);

    int GetQuantity(ItemId id) const;
    int GetUniqueItemCount() const;
    int GetTotalItemCount() const;
    int GetCountByCategory(ItemCategory category) const;

    const std::vector<InventoryEntry>& GetEntries() const;

private:
    std::vector<InventoryEntry> entries;
};

} // namespace items
