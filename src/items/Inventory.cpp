#include "items/Inventory.h"

#include <algorithm>

namespace items {

bool Inventory::CanAddItem(ItemId id, int quantity) const {
    if (quantity <= 0) {
        return false;
    }

    const ItemDefinition& definition = GetItemDefinition(id);
    const int currentQuantity = GetQuantity(id);
    return currentQuantity < definition.maxStack;
}

void Inventory::Clear() {
    entries.clear();
}

void Inventory::SetEntries(const std::vector<InventoryEntry>& value) {
    entries.clear();
    for (const InventoryEntry& entry : value) {
        AddItem(entry.id, entry.quantity);
    }
}

void Inventory::AddItem(ItemId id, int quantity) {
    if (quantity <= 0) {
        return;
    }

    const ItemDefinition& definition = GetItemDefinition(id);
    auto it = std::find_if(entries.begin(), entries.end(), [id](const InventoryEntry& entry) {
        return entry.id == id;
    });

    if (it == entries.end()) {
        entries.push_back(InventoryEntry{id, std::min(quantity, definition.maxStack)});
        return;
    }

    it->quantity = std::min(definition.maxStack, it->quantity + quantity);
}

bool Inventory::RemoveItem(ItemId id, int quantity) {
    if (quantity <= 0) {
        return false;
    }

    auto it = std::find_if(entries.begin(), entries.end(), [id](const InventoryEntry& entry) {
        return entry.id == id;
    });

    if (it == entries.end() || it->quantity < quantity) {
        return false;
    }

    it->quantity -= quantity;
    if (it->quantity <= 0) {
        entries.erase(it);
    }

    return true;
}

int Inventory::GetQuantity(ItemId id) const {
    auto it = std::find_if(entries.begin(), entries.end(), [id](const InventoryEntry& entry) {
        return entry.id == id;
    });

    return it == entries.end() ? 0 : it->quantity;
}

int Inventory::GetUniqueItemCount() const {
    return static_cast<int>(entries.size());
}

int Inventory::GetTotalItemCount() const {
    int total = 0;
    for (const InventoryEntry& entry : entries) {
        total += entry.quantity;
    }
    return total;
}

int Inventory::GetCountByCategory(ItemCategory category) const {
    int total = 0;
    for (const InventoryEntry& entry : entries) {
        if (GetItemDefinition(entry.id).category == category) {
            total += entry.quantity;
        }
    }
    return total;
}

const std::vector<InventoryEntry>& Inventory::GetEntries() const {
    return entries;
}

} // namespace items
