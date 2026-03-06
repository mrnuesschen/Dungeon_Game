#pragma once

#include <array>
#include <optional>
#include <vector>

#include "items/ItemSystem.h"

namespace items {

class EquipmentLoadout {
public:
    EquipmentLoadout() : slots{} {}

    bool Equip(ItemId id, EquipmentSlot slot) {
        if (!CanEquipInSlot(id, slot)) {
            return false;
        }
        slots[static_cast<size_t>(slot)] = id;
        return true;
    }

    bool Unequip(EquipmentSlot slot) {
        const size_t index = static_cast<size_t>(slot);
        if (!slots[index].has_value()) {
            return false;
        }

        slots[index] = std::nullopt;
        return true;
    }

    std::optional<ItemId> GetEquippedItem(EquipmentSlot slot) const {
        return slots[static_cast<size_t>(slot)];
    }

    bool IsSlotOccupied(EquipmentSlot slot) const {
        return slots[static_cast<size_t>(slot)].has_value();
    }

    int CountEquippedCopies(ItemId id) const {
        int count = 0;
        for (const std::optional<ItemId>& equipped : slots) {
            if (equipped.has_value() && *equipped == id) {
                count++;
            }
        }
        return count;
    }

    void Clear() {
        for (std::optional<ItemId>& equipped : slots) {
            equipped = std::nullopt;
        }
    }

    std::vector<EquipmentSlot> GetOccupiedSlots() const {
        std::vector<EquipmentSlot> result;
        for (int i = 0; i < static_cast<int>(EquipmentSlot::Count); ++i) {
            const EquipmentSlot slot = static_cast<EquipmentSlot>(i);
            if (IsSlotOccupied(slot)) {
                result.push_back(slot);
            }
        }
        return result;
    }

private:
    std::array<std::optional<ItemId>, static_cast<size_t>(EquipmentSlot::Count)> slots;
};

} // namespace items
