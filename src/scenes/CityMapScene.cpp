#include "scenes/CityMapScene.h"

#include <algorithm>

#include "raylib.h"

namespace {

const char* PlayerClassToString(PlayerClass playerClass) {
    switch (playerClass) {
    case PlayerClass::Tank:
        return "Tank";
    case PlayerClass::Knight:
        return "Knight";
    case PlayerClass::Assassin:
        return "Assassin";
    case PlayerClass::Mage:
        return "Mage";
    case PlayerClass::Healer:
        return "Healer";
    case PlayerClass::Alchemist:
        return "Alchemist";
    }
    return "Adventurer";
}

const char* ItemCategoryToString(items::ItemCategory category) {
    switch (category) {
    case items::ItemCategory::Equipment:
        return "Equipment";
    case items::ItemCategory::Consumable:
        return "Consumable";
    case items::ItemCategory::Quest:
        return "Quest";
    }
    return "Unknown";
}

void CloseAllBuildings(bool& tavernOpen, bool& guildOpen, bool& shopOpen, bool& innOpen) {
    tavernOpen = false;
    guildOpen = false;
    shopOpen = false;
    innOpen = false;
}

const std::array<items::EquipmentSlot, 15> kInnEquipmentSlots{
    items::EquipmentSlot::Head,
    items::EquipmentSlot::Necklace,
    items::EquipmentSlot::Hand1,
    items::EquipmentSlot::Hand2,
    items::EquipmentSlot::UpperBodyInner,
    items::EquipmentSlot::UpperBodyOuter,
    items::EquipmentSlot::Underpants,
    items::EquipmentSlot::Pants,
    items::EquipmentSlot::Shoes,
    items::EquipmentSlot::Jewelry1,
    items::EquipmentSlot::Jewelry2,
    items::EquipmentSlot::Jewelry3,
    items::EquipmentSlot::Jewelry4,
    items::EquipmentSlot::Weapon1,
    items::EquipmentSlot::Weapon2,
};

} // namespace

CityMapScene::CityMapScene()
    : tavernButton{Rectangle{120.0f, 150.0f, 220.0f, 64.0f}, "Tavern", true},
      guildButton{Rectangle{360.0f, 150.0f, 220.0f, 64.0f}, "Guild", true},
      shopButton{Rectangle{120.0f, 250.0f, 220.0f, 64.0f}, "Shop", true},
      innButton{Rectangle{360.0f, 250.0f, 220.0f, 64.0f}, "Inn", true},
      worldButton{Rectangle{700.0f, 440.0f, 220.0f, 56.0f}, "Back To World", true},
      closeBuildingButton{Rectangle{680.0f, 40.0f, 240.0f, 54.0f}, "Back To City", true},
      innSaveButton{Rectangle{80.0f, 430.0f, 300.0f, 48.0f}, "Rest And Save", true},
            innEquipmentButton{Rectangle{390.0f, 430.0f, 220.0f, 48.0f}, "Equipment", true},
      guildViewToggleButton{Rectangle{650.0f, 96.0f, 240.0f, 42.0f}, "Show Accepted", true},
            guildOfferScrollUpButton{Rectangle{880.0f, 176.0f, 26.0f, 30.0f}, "^", true},
            guildOfferScrollDownButton{Rectangle{880.0f, 336.0f, 26.0f, 30.0f}, "v", true},
            guildAcceptButtons{ui::Button{Rectangle{760.0f, 228.0f, 130.0f, 34.0f}, "Accept", true},
                                                 ui::Button{Rectangle{760.0f, 358.0f, 130.0f, 34.0f}, "Accept", true}},
      shopScrollUpButton{Rectangle{460.0f, 196.0f, 40.0f, 40.0f}, "^", true},
      shopScrollDownButton{Rectangle{460.0f, 396.0f, 40.0f, 40.0f}, "v", true},
      shopVisibleButtons{ui::Button{Rectangle{80.0f, 196.0f, 380.0f, 44.0f}, "", true},
                         ui::Button{Rectangle{80.0f, 246.0f, 380.0f, 44.0f}, "", true},
                         ui::Button{Rectangle{80.0f, 296.0f, 380.0f, 44.0f}, "", true},
                         ui::Button{Rectangle{80.0f, 346.0f, 380.0f, 44.0f}, "", true},
                         ui::Button{Rectangle{80.0f, 396.0f, 380.0f, 44.0f}, "", true}},
      tavernOpen(false),
      shopOpen(false),
      guildOpen(false),
      guildShowAcceptedList(false),
      innOpen(false),
            innEquipmentView(false),
            guildOfferScrollOffset(0),
      shopScrollOffset(0),
      innPlayerName("Hero"),
      innPlayerClass(PlayerClass::Knight),
      innPlayerLevel(1),
      innPlayerHp(120),
      innInventoryEntries(),
            innEquipmentLoadout(),
            innDraggedItem(std::nullopt),
            displayedGuildQuests(),
      questLog(),
      playerGold(0),
      requestedShopItem(std::nullopt),
            requestedEquipItem(std::nullopt),
    requestedUnequipSlot(std::nullopt),
      hoveredShopItem(std::nullopt),
            infoText("Choose a place in the city.") {
        RefreshGuildQuestSelection();
}

void CityMapScene::SetPlayerGold(int value) {
    playerGold = std::max(0, value);
}

void CityMapScene::SetInnSnapshot(const BattleSaveData& saveData) {
    innPlayerName = saveData.playerName.empty() ? "Hero" : saveData.playerName;
    innPlayerClass = saveData.playerClass;
    innPlayerLevel = std::max(1, saveData.playerLevel);
    innPlayerHp = std::max(0, saveData.playerHp);

    innInventoryEntries.clear();
    const int clampedCount = std::clamp(saveData.inventoryEntryCount, 0, kMaxInventorySaveEntries);
    innInventoryEntries.reserve(static_cast<size_t>(clampedCount));
    for (int i = 0; i < clampedCount; ++i) {
        const int quantity = saveData.inventoryQuantity[static_cast<size_t>(i)];
        if (quantity <= 0) {
            continue;
        }

        const int rawItemId = saveData.inventoryItemId[static_cast<size_t>(i)];
        if (!items::IsValidItemIdValue(rawItemId)) {
            continue;
        }

        innInventoryEntries.push_back(items::InventoryEntry{static_cast<items::ItemId>(rawItemId), quantity});
    }
}

void CityMapScene::SetInnEquipmentSnapshot(const items::EquipmentLoadout& loadout) {
    innEquipmentLoadout = loadout;
}

void CityMapScene::SetShopFeedback(const std::string& value) {
    if (!value.empty()) {
        infoText = value;
    }
}

void CityMapScene::RegisterBattleVictoryForQuests(int floor, const std::vector<EnemyArchetype>& defeatedEnemies) {
    questLog.RegisterBattleVictory(floor, defeatedEnemies);
}

void CityMapScene::RegisterQuestItemCollected(quests::QuestId questId, items::ItemId itemId, int floor) {
    const bool wasCompleted = questLog.IsCompleted(questId);
    questLog.RegisterQuestItemCollected(questId, itemId, floor);
    if (!wasCompleted && questLog.IsCompleted(questId)) {
        const quests::QuestDefinition& quest = quests::GetQuestDefinition(questId);
        infoText = std::string("Quest complete: ") + quest.title + ".";
    }
}

std::vector<quests::QuestItemSpawn> CityMapScene::GetPendingQuestItemSpawnsForFloor(int floor) const {
    return questLog.GetPendingQuestItemSpawnsForFloor(floor);
}

void CityMapScene::RefreshGuildQuestSelection() {
    displayedGuildQuests.clear();

    std::vector<quests::QuestDefinition> availableQuests;
    const std::vector<quests::QuestDefinition>& allQuests = quests::GetAllQuestDefinitions();
    availableQuests.reserve(allQuests.size());
    for (const quests::QuestDefinition& quest : allQuests) {
        if (!questLog.IsAccepted(quest.id)) {
            availableQuests.push_back(quest);
        }
    }

    for (int i = static_cast<int>(availableQuests.size()) - 1; i > 0; --i) {
        const int j = GetRandomValue(0, i);
        std::swap(availableQuests[static_cast<size_t>(i)], availableQuests[static_cast<size_t>(j)]);
    }

    const int limit = std::min(4, static_cast<int>(availableQuests.size()));
    displayedGuildQuests.reserve(static_cast<size_t>(limit));
    for (int i = 0; i < limit; ++i) {
        displayedGuildQuests.push_back(availableQuests[static_cast<size_t>(i)]);
    }

    const int maxOffset = std::max(0, static_cast<int>(displayedGuildQuests.size()) - 2);
    guildOfferScrollOffset = std::clamp(guildOfferScrollOffset, 0, maxOffset);
}

std::optional<items::ItemId> CityMapScene::ConsumeRequestedShopItem() {
    std::optional<items::ItemId> result = requestedShopItem;
    requestedShopItem = std::nullopt;
    return result;
}

std::optional<CityMapEquipRequest> CityMapScene::ConsumeRequestedEquipItem() {
    std::optional<CityMapEquipRequest> result = requestedEquipItem;
    requestedEquipItem = std::nullopt;
    return result;
}

std::optional<items::EquipmentSlot> CityMapScene::ConsumeRequestedUnequipSlot() {
    std::optional<items::EquipmentSlot> result = requestedUnequipSlot;
    requestedUnequipSlot = std::nullopt;
    return result;
}

CityMapAction CityMapScene::Update() {
    const float sx = static_cast<float>(GetScreenWidth()) / 960.0f;
    const float sy = static_cast<float>(GetScreenHeight()) / 540.0f;

    tavernButton.bounds = Rectangle{120.0f * sx, 150.0f * sy, 220.0f * sx, 64.0f * sy};
    guildButton.bounds = Rectangle{360.0f * sx, 150.0f * sy, 220.0f * sx, 64.0f * sy};
    shopButton.bounds = Rectangle{120.0f * sx, 250.0f * sy, 220.0f * sx, 64.0f * sy};
    innButton.bounds = Rectangle{360.0f * sx, 250.0f * sy, 220.0f * sx, 64.0f * sy};
    worldButton.bounds = Rectangle{700.0f * sx, 440.0f * sy, 220.0f * sx, 56.0f * sy};

    closeBuildingButton.bounds = Rectangle{680.0f * sx, 40.0f * sy, 240.0f * sx, 54.0f * sy};
    if (innOpen && innEquipmentView) {
        innSaveButton.bounds = Rectangle{0.0f, 0.0f, 0.0f, 0.0f};
    } else {
        innSaveButton.bounds = Rectangle{80.0f * sx, 430.0f * sy, 300.0f * sx, 48.0f * sy};
    }
    innEquipmentButton.bounds = Rectangle{390.0f * sx, 430.0f * sy, 220.0f * sx, 48.0f * sy};
    guildViewToggleButton.bounds = Rectangle{650.0f * sx, 96.0f * sy, 240.0f * sx, 42.0f * sy};
    guildOfferScrollUpButton.bounds = Rectangle{880.0f * sx, 176.0f * sy, 26.0f * sx, 30.0f * sy};
    guildOfferScrollDownButton.bounds = Rectangle{880.0f * sx, 336.0f * sy, 26.0f * sx, 30.0f * sy};
    for (int i = 0; i < 2; ++i) {
        guildAcceptButtons[static_cast<size_t>(i)].bounds = Rectangle{760.0f * sx, (228.0f + static_cast<float>(i) * 130.0f) * sy, 130.0f * sx, 34.0f * sy};
    }
    shopScrollUpButton.bounds = Rectangle{460.0f * sx, 196.0f * sy, 40.0f * sx, 40.0f * sy};
    shopScrollDownButton.bounds = Rectangle{460.0f * sx, 396.0f * sy, 40.0f * sx, 40.0f * sy};
    for (int i = 0; i < 5; ++i) {
        shopVisibleButtons[static_cast<size_t>(i)].bounds = Rectangle{80.0f * sx, (196.0f + static_cast<float>(i) * 50.0f) * sy, 380.0f * sx, 44.0f * sy};
    }

    const bool buildingOpen = tavernOpen || guildOpen || shopOpen || innOpen;
    if (!buildingOpen) {
        if (ui::IsPressed(tavernButton)) {
            CloseAllBuildings(tavernOpen, guildOpen, shopOpen, innOpen);
            tavernOpen = true;
            infoText = "Tavern: Quiet for now. Rumors will return soon.";
            return CityMapAction::None;
        }
        if (ui::IsPressed(guildButton)) {
            CloseAllBuildings(tavernOpen, guildOpen, shopOpen, innOpen);
            guildOpen = true;
            guildShowAcceptedList = false;
            guildOfferScrollOffset = 0;
            RefreshGuildQuestSelection();
            infoText = "Guild: Browse upcoming contracts.";
            return CityMapAction::None;
        }
        if (ui::IsPressed(shopButton)) {
            CloseAllBuildings(tavernOpen, guildOpen, shopOpen, innOpen);
            shopOpen = true;
            shopScrollOffset = 0;
            hoveredShopItem = std::nullopt;
            infoText = "Shop: Hover an item to see details.";
            return CityMapAction::None;
        }
        if (ui::IsPressed(innButton)) {
            CloseAllBuildings(tavernOpen, guildOpen, shopOpen, innOpen);
            innOpen = true;
            innEquipmentView = false;
            innDraggedItem = std::nullopt;
            infoText = "Inn: Review your hero and inventory, then rest.";
            return CityMapAction::None;
        }
        if (ui::IsPressed(worldButton)) {
            return CityMapAction::BackToWorld;
        }
        return CityMapAction::None;
    }

    if (ui::IsPressed(closeBuildingButton)) {
        CloseAllBuildings(tavernOpen, guildOpen, shopOpen, innOpen);
        hoveredShopItem = std::nullopt;
        infoText = "Back in the city center.";
        return CityMapAction::None;
    }

    if (guildOpen) {
        if (ui::IsPressed(guildViewToggleButton)) {
            guildShowAcceptedList = !guildShowAcceptedList;
        }

        if (!guildShowAcceptedList) {
            const int total = static_cast<int>(displayedGuildQuests.size());
            const int visible = 2;
            const int maxOffset = std::max(0, total - visible);
            if (ui::IsPressed(guildOfferScrollUpButton)) {
                guildOfferScrollOffset = std::max(0, guildOfferScrollOffset - 1);
            }
            if (ui::IsPressed(guildOfferScrollDownButton)) {
                guildOfferScrollOffset = std::min(maxOffset, guildOfferScrollOffset + 1);
            }

            const float wheel = GetMouseWheelMove();
            if (wheel < 0.0f) {
                guildOfferScrollOffset = std::min(maxOffset, guildOfferScrollOffset + 1);
            } else if (wheel > 0.0f) {
                guildOfferScrollOffset = std::max(0, guildOfferScrollOffset - 1);
            }

            const int cardsToHandle = std::max(0, std::min(visible, total - guildOfferScrollOffset));
            for (int i = 0; i < cardsToHandle; ++i) {
                const quests::QuestDefinition& quest = displayedGuildQuests[static_cast<size_t>(guildOfferScrollOffset + i)];
                if (!ui::IsPressed(guildAcceptButtons[static_cast<size_t>(i)])) {
                    continue;
                }

                if (questLog.IsCompleted(quest.id)) {
                    infoText = std::string("Quest already completed: ") + quest.title + ".";
                } else if (questLog.IsAccepted(quest.id)) {
                    const int progress = questLog.GetProgress(quest.id);
                    const int required = questLog.GetRequiredCount(quest.id);
                    infoText = std::string("Quest in progress: ") + quest.title + " (" + std::to_string(progress) + "/" + std::to_string(required) + ").";
                } else if (questLog.AcceptQuest(quest.id)) {
                    infoText = std::string("Accepted quest: ") + quest.title + ".";
                    RefreshGuildQuestSelection();
                }
            }
        }
    }

    if (innOpen) {
        ui::Button effectiveEquipmentButton = innEquipmentButton;
        if (innEquipmentView) {
            effectiveEquipmentButton.bounds = Rectangle{650.0f * sx, 40.0f * sy, 240.0f * sx, 42.0f * sy};
        }

        if (ui::IsPressed(effectiveEquipmentButton)) {
            innEquipmentView = !innEquipmentView;
            innDraggedItem = std::nullopt;
            infoText = innEquipmentView ? "Inn: Drag items onto highlighted equipment slots." : "Inn: Character overview.";
        }

        if (!innEquipmentView && ui::IsPressed(innSaveButton)) {
            infoText = "Inn: Rest complete. You can save your progress.";
            return CityMapAction::SaveAtInn;
        }
    }

    if (innOpen && innEquipmentView) {
        std::array<Rectangle, kInnEquipmentSlots.size()> slotRects{};
        const float slotStartX = 58.0f * sx;
        const float slotStartY = 128.0f * sy;
        const float slotW = 108.0f * sx;
        const float slotH = 50.0f * sy;
        const float slotGapX = 6.0f * sx;
        const float slotGapY = 6.0f * sy;
        for (size_t i = 0; i < kInnEquipmentSlots.size(); ++i) {
            const int row = static_cast<int>(i) / 3;
            const int col = static_cast<int>(i) % 3;
            slotRects[i] = Rectangle{slotStartX + static_cast<float>(col) * (slotW + slotGapX), slotStartY + static_cast<float>(row) * (slotH + slotGapY), slotW, slotH};
        }

        const float inventoryX = 500.0f * sx;
        const float inventoryY = 128.0f * sy;
        const float inventoryW = 400.0f * sx;
        const float inventoryRowH = 28.0f * sy;
        const int inventoryRows = 11;
        const int visibleItems = std::min(inventoryRows, static_cast<int>(innInventoryEntries.size()));

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !innDraggedItem.has_value()) {
            const Vector2 mouse = GetMousePosition();
            for (int i = 0; i < visibleItems; ++i) {
                const Rectangle rowRect{inventoryX, inventoryY + static_cast<float>(i) * (inventoryRowH + 4.0f * sy), inventoryW, inventoryRowH};
                if (CheckCollisionPointRec(mouse, rowRect)) {
                    innDraggedItem = innInventoryEntries[static_cast<size_t>(i)].id;
                    break;
                }
            }
        }

        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            const Vector2 mouse = GetMousePosition();
            for (size_t i = 0; i < kInnEquipmentSlots.size(); ++i) {
                if (!CheckCollisionPointRec(mouse, slotRects[i])) {
                    continue;
                }

                const items::EquipmentSlot slot = kInnEquipmentSlots[i];
                if (!innEquipmentLoadout.IsSlotOccupied(slot)) {
                    continue;
                }

                requestedUnequipSlot = slot;
                innDraggedItem = std::nullopt;
                return CityMapAction::UnequipItemAtInn;
            }
        }

        if (innDraggedItem.has_value() && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            const Vector2 mouse = GetMousePosition();
            for (size_t i = 0; i < kInnEquipmentSlots.size(); ++i) {
                if (!CheckCollisionPointRec(mouse, slotRects[i])) {
                    continue;
                }

                const items::EquipmentSlot slot = kInnEquipmentSlots[i];
                if (!items::CanEquipInSlot(*innDraggedItem, slot)) {
                    continue;
                }

                requestedEquipItem = CityMapEquipRequest{*innDraggedItem, slot};
                innDraggedItem = std::nullopt;
                return CityMapAction::EquipItemAtInn;
            }

            innDraggedItem = std::nullopt;
        }
    }

    if (shopOpen) {
        const std::vector<items::ItemId>& shopItems = items::GetShopItemIds();
        const int totalShopItems = static_cast<int>(shopItems.size());
        const int visibleShopItems = static_cast<int>(shopVisibleButtons.size());
        const int maxScrollOffset = std::max(0, totalShopItems - visibleShopItems);

        if (ui::IsPressed(shopScrollUpButton)) {
            shopScrollOffset = std::max(0, shopScrollOffset - 1);
        }
        if (ui::IsPressed(shopScrollDownButton)) {
            shopScrollOffset = std::min(maxScrollOffset, shopScrollOffset + 1);
        }

        const float wheel = GetMouseWheelMove();
        if (wheel < 0.0f) {
            shopScrollOffset = std::min(maxScrollOffset, shopScrollOffset + 1);
        } else if (wheel > 0.0f) {
            shopScrollOffset = std::max(0, shopScrollOffset - 1);
        }

        hoveredShopItem = std::nullopt;
        for (int slot = 0; slot < visibleShopItems; ++slot) {
            const int index = shopScrollOffset + slot;
            ui::Button& button = shopVisibleButtons[static_cast<size_t>(slot)];
            if (index >= totalShopItems) {
                button.enabled = false;
                button.text.clear();
                continue;
            }

            const items::ItemId itemId = shopItems[static_cast<size_t>(index)];
            const items::ItemDefinition& definition = items::GetItemDefinition(itemId);
            button.enabled = definition.buyPrice > 0;
            button.text = std::string("Buy ") + definition.name + " (" + std::to_string(definition.buyPrice) + "g)";

            if (CheckCollisionPointRec(GetMousePosition(), button.bounds)) {
                hoveredShopItem = itemId;
            }

            if (ui::IsPressed(button)) {
                requestedShopItem = itemId;
                return CityMapAction::BuyItem;
            }
        }
    }

    return CityMapAction::None;
}

void CityMapScene::Draw() const {
    const float sx = static_cast<float>(GetScreenWidth()) / 960.0f;
    const float sy = static_cast<float>(GetScreenHeight()) / 540.0f;
    const float sf = std::min(sx, sy);

    const bool buildingOpen = tavernOpen || guildOpen || shopOpen || innOpen;
    if (!buildingOpen) {
        DrawText("City Map", static_cast<int>(36.0f * sx), static_cast<int>(30.0f * sy), static_cast<int>(50.0f * sf), RAYWHITE);
        DrawText("Districts", static_cast<int>(42.0f * sx), static_cast<int>(88.0f * sy), static_cast<int>(24.0f * sf), LIGHTGRAY);
        DrawText(TextFormat("Gold: %d", playerGold), static_cast<int>(620.0f * sx), static_cast<int>(92.0f * sy), static_cast<int>(24.0f * sf), Color{240, 212, 120, 255});

        DrawRectangle(static_cast<int>(90.0f * sx), static_cast<int>(130.0f * sy), static_cast<int>(520.0f * sx), static_cast<int>(220.0f * sy), Color{28, 34, 50, 190});
        DrawRectangleLines(static_cast<int>(90.0f * sx), static_cast<int>(130.0f * sy), static_cast<int>(520.0f * sx), static_cast<int>(220.0f * sy), Color{120, 140, 175, 255});

        ui::DrawButton(tavernButton);
        ui::DrawButton(guildButton);
        ui::DrawButton(shopButton);
        ui::DrawButton(innButton);
        ui::DrawButton(worldButton);

        DrawText(infoText.c_str(), static_cast<int>(40.0f * sx), static_cast<int>(498.0f * sy), static_cast<int>(18.0f * sf), Color{205, 210, 220, 255});
        return;
    }

    DrawRectangle(static_cast<int>(30.0f * sx), static_cast<int>(24.0f * sy), static_cast<int>(900.0f * sx), static_cast<int>(492.0f * sy), Color{18, 24, 36, 235});
    DrawRectangleLines(static_cast<int>(30.0f * sx), static_cast<int>(24.0f * sy), static_cast<int>(900.0f * sx), static_cast<int>(492.0f * sy), Color{120, 140, 175, 255});

    ui::DrawButton(closeBuildingButton);
    const bool hideTopHeaderForInnEquipment = innOpen && innEquipmentView;
    if (!hideTopHeaderForInnEquipment) {
        DrawText(TextFormat("Gold: %d", playerGold), static_cast<int>(50.0f * sx), static_cast<int>(48.0f * sy), static_cast<int>(24.0f * sf), Color{240, 212, 120, 255});
    }

    if (tavernOpen) {
        DrawText("Tavern", static_cast<int>(50.0f * sx), static_cast<int>(92.0f * sy), static_cast<int>(42.0f * sf), RAYWHITE);
        DrawText("The tavern is calm right now.", static_cast<int>(54.0f * sx), static_cast<int>(170.0f * sy), static_cast<int>(24.0f * sf), Color{205, 210, 220, 255});
        DrawText("For now, only the return button is available.", static_cast<int>(54.0f * sx), static_cast<int>(204.0f * sy), static_cast<int>(20.0f * sf), Color{185, 195, 210, 255});
    }

    if (guildOpen) {
        DrawText("Guild Contracts", static_cast<int>(50.0f * sx), static_cast<int>(92.0f * sy), static_cast<int>(42.0f * sf), RAYWHITE);

        ui::Button guildToggleButton = guildViewToggleButton;
        guildToggleButton.text = guildShowAcceptedList ? "Show Offers" : "Show Accepted";
        ui::DrawButton(guildToggleButton);

        if (guildShowAcceptedList) {
            std::vector<quests::QuestDefinition> acceptedQuests;
            const std::vector<quests::QuestDefinition>& allQuests = quests::GetAllQuestDefinitions();
            for (const quests::QuestDefinition& quest : allQuests) {
                if (questLog.IsAccepted(quest.id)) {
                    acceptedQuests.push_back(quest);
                }
            }

            if (acceptedQuests.empty()) {
                DrawText("No accepted quests yet.", static_cast<int>(62.0f * sx), static_cast<int>(170.0f * sy), static_cast<int>(22.0f * sf), Color{195, 205, 220, 255});
                DrawText("Accept contracts from Show Offers.", static_cast<int>(62.0f * sx), static_cast<int>(202.0f * sy), static_cast<int>(18.0f * sf), Color{180, 190, 205, 255});
            } else {
                const int cardsToDraw = std::min(3, static_cast<int>(acceptedQuests.size()));
                for (int i = 0; i < cardsToDraw; ++i) {
                    const quests::QuestDefinition& quest = acceptedQuests[static_cast<size_t>(i)];
                    const float cardY = 150.0f + static_cast<float>(i) * 110.0f;
                    const bool completed = questLog.IsCompleted(quest.id);
                    const int progress = questLog.GetProgress(quest.id);
                    const int required = questLog.GetRequiredCount(quest.id);

                    DrawRectangle(static_cast<int>(50.0f * sx), static_cast<int>(cardY * sy), static_cast<int>(860.0f * sx), static_cast<int>(100.0f * sy), Color{28, 34, 50, 200});

                    DrawText(TextFormat("Quest: %s", quest.title), static_cast<int>(62.0f * sx), static_cast<int>((cardY + 12.0f) * sy), static_cast<int>(24.0f * sf), RAYWHITE);
                    DrawText(TextFormat("Target: %s", quest.target), static_cast<int>(62.0f * sx), static_cast<int>((cardY + 42.0f) * sy), static_cast<int>(18.0f * sf), Color{195, 205, 220, 255});
                    DrawText(TextFormat("Reward: %s", quest.reward), static_cast<int>(610.0f * sx), static_cast<int>((cardY + 64.0f) * sy), static_cast<int>(18.0f * sf), Color{240, 212, 120, 255});
                    DrawText(TextFormat("Progress: %d/%d", progress, required), static_cast<int>(610.0f * sx), static_cast<int>((cardY + 42.0f) * sy), static_cast<int>(18.0f * sf), Color{190, 205, 220, 255});
                    DrawText(completed ? "Status: Completed" : "Status: In Progress", static_cast<int>(610.0f * sx), static_cast<int>((cardY + 20.0f) * sy), static_cast<int>(18.0f * sf), completed ? Color{120, 220, 140, 255} : Color{180, 190, 205, 255});
                }
            }

            DrawText("Accepted Quest List", static_cast<int>(50.0f * sx), static_cast<int>(126.0f * sy), static_cast<int>(20.0f * sf), Color{195, 205, 220, 255});
        } else if (displayedGuildQuests.empty()) {
            DrawText("No contracts available.", static_cast<int>(62.0f * sx), static_cast<int>(170.0f * sy), static_cast<int>(22.0f * sf), Color{195, 205, 220, 255});
        } else {
            const int total = static_cast<int>(displayedGuildQuests.size());
            const int visible = 2;
            const int maxOffset = std::max(0, total - visible);
            const int cardsToDraw = std::max(0, std::min(visible, total - guildOfferScrollOffset));

            ui::Button scrollUpButton = guildOfferScrollUpButton;
            ui::Button scrollDownButton = guildOfferScrollDownButton;
            scrollUpButton.enabled = guildOfferScrollOffset > 0;
            scrollDownButton.enabled = guildOfferScrollOffset < maxOffset;
            ui::DrawButton(scrollUpButton);
            ui::DrawButton(scrollDownButton);

            DrawText(TextFormat("Showing %d-%d of %d", guildOfferScrollOffset + 1,
                                guildOfferScrollOffset + cardsToDraw, total),
                     static_cast<int>(740.0f * sx), static_cast<int>(136.0f * sy),
                     static_cast<int>(16.0f * sf), Color{180, 190, 205, 255});

            for (int i = 0; i < cardsToDraw; ++i) {
                const quests::QuestDefinition& quest = displayedGuildQuests[static_cast<size_t>(guildOfferScrollOffset + i)];
                const float cardY = 180.0f + static_cast<float>(i) * 130.0f;
                const bool accepted = questLog.IsAccepted(quest.id);
                const bool completed = questLog.IsCompleted(quest.id);
                const int progress = questLog.GetProgress(quest.id);
                const int required = questLog.GetRequiredCount(quest.id);

                ui::Button acceptButton = guildAcceptButtons[static_cast<size_t>(i)];
                if (completed) {
                    acceptButton.text = "Completed";
                    acceptButton.enabled = false;
                } else if (accepted) {
                    acceptButton.text = "Accepted";
                    acceptButton.enabled = false;
                } else {
                    acceptButton.text = "Accept";
                    acceptButton.enabled = true;
                }

                DrawRectangle(static_cast<int>(50.0f * sx), static_cast<int>(cardY * sy), static_cast<int>(860.0f * sx), static_cast<int>(100.0f * sy), Color{28, 34, 50, 200});

                DrawText(TextFormat("Quest: %s", quest.title), static_cast<int>(62.0f * sx), static_cast<int>((cardY + 12.0f) * sy), static_cast<int>(24.0f * sf), RAYWHITE);
                DrawText(TextFormat("Target: %s", quest.target), static_cast<int>(62.0f * sx), static_cast<int>((cardY + 42.0f) * sy), static_cast<int>(18.0f * sf), Color{195, 205, 220, 255});
                DrawText(TextFormat("Description: %s", quest.description), static_cast<int>(62.0f * sx), static_cast<int>((cardY + 64.0f) * sy), static_cast<int>(18.0f * sf), Color{180, 190, 205, 255});
                DrawText(TextFormat("Reward: %s", quest.reward), static_cast<int>(610.0f * sx), static_cast<int>((cardY + 64.0f) * sy), static_cast<int>(18.0f * sf), Color{240, 212, 120, 255});
                DrawText(TextFormat("Progress: %d/%d", progress, required), static_cast<int>(610.0f * sx), static_cast<int>((cardY + 42.0f) * sy), static_cast<int>(18.0f * sf), Color{190, 205, 220, 255});

                ui::DrawButton(acceptButton);
            }
        }
    }

    if (innOpen) {
        if (!innEquipmentView) {
            DrawText("Inn", static_cast<int>(50.0f * sx), static_cast<int>(92.0f * sy), static_cast<int>(42.0f * sf), RAYWHITE);
        }
        ui::Button equipmentButton = innEquipmentButton;
        equipmentButton.text = innEquipmentView ? "Back To Inn" : "Equipment";
        if (innEquipmentView) {
            equipmentButton.bounds = Rectangle{650.0f * sx, 40.0f * sy, 240.0f * sx, 42.0f * sy};
        }

        if (!innEquipmentView) {
            DrawRectangle(static_cast<int>(50.0f * sx), static_cast<int>(150.0f * sy), static_cast<int>(860.0f * sx), static_cast<int>(320.0f * sy), Color{28, 34, 50, 200});
            DrawRectangleLines(static_cast<int>(50.0f * sx), static_cast<int>(150.0f * sy), static_cast<int>(860.0f * sx), static_cast<int>(320.0f * sy), Color{96, 112, 140, 255});

            DrawText("Character", static_cast<int>(64.0f * sx), static_cast<int>(170.0f * sy), static_cast<int>(28.0f * sf), RAYWHITE);
            DrawText(TextFormat("Name: %s", innPlayerName.c_str()), static_cast<int>(64.0f * sx), static_cast<int>(212.0f * sy), static_cast<int>(22.0f * sf), Color{205, 210, 220, 255});
            DrawText(TextFormat("Class: %s", PlayerClassToString(innPlayerClass)), static_cast<int>(64.0f * sx), static_cast<int>(246.0f * sy), static_cast<int>(22.0f * sf), Color{205, 210, 220, 255});
            DrawText(TextFormat("Level: %d", innPlayerLevel), static_cast<int>(64.0f * sx), static_cast<int>(280.0f * sy), static_cast<int>(22.0f * sf), Color{205, 210, 220, 255});
            DrawText(TextFormat("Current HP: %d", innPlayerHp), static_cast<int>(64.0f * sx), static_cast<int>(314.0f * sy), static_cast<int>(22.0f * sf), Color{205, 210, 220, 255});
            DrawText("Click Equipment to manage gear.", static_cast<int>(64.0f * sx), static_cast<int>(392.0f * sy), static_cast<int>(18.0f * sf), Color{190, 205, 220, 255});
        } else {
            const float panelY = 96.0f * sy;
            const float panelH = 386.0f * sy;
            const float leftX = 50.0f * sx;
            const float gap = 20.0f * sx;
            const float panelW = (860.0f * sx - gap) * 0.5f;
            const float rightX = leftX + panelW + gap;

            DrawRectangle(static_cast<int>(leftX), static_cast<int>(panelY), static_cast<int>(panelW), static_cast<int>(panelH), Color{28, 34, 50, 200});
            DrawRectangleLines(static_cast<int>(leftX), static_cast<int>(panelY), static_cast<int>(panelW), static_cast<int>(panelH), Color{96, 112, 140, 255});
            DrawText("Equipment", static_cast<int>((leftX + 12.0f * sx)), static_cast<int>((panelY + 12.0f * sy)), static_cast<int>(22.0f * sf), RAYWHITE);

            DrawRectangle(static_cast<int>(rightX), static_cast<int>(panelY), static_cast<int>(panelW), static_cast<int>(panelH), Color{28, 34, 50, 200});
            DrawRectangleLines(static_cast<int>(rightX), static_cast<int>(panelY), static_cast<int>(panelW), static_cast<int>(panelH), Color{96, 112, 140, 255});
            DrawText("Inventory (drag)", static_cast<int>((rightX + 12.0f * sx)), static_cast<int>((panelY + 12.0f * sy)), static_cast<int>(20.0f * sf), RAYWHITE);

            std::array<Rectangle, kInnEquipmentSlots.size()> slotRects{};
            const float slotStartX = leftX + 8.0f * sx;
            const float slotStartY = 128.0f * sy;
            const float slotW = 108.0f * sx;
            const float slotH = 50.0f * sy;
            const float slotGapX = 6.0f * sx;
            const float slotGapY = 6.0f * sy;
            for (size_t i = 0; i < kInnEquipmentSlots.size(); ++i) {
                const int row = static_cast<int>(i) / 3;
                const int col = static_cast<int>(i) % 3;
                const items::EquipmentSlot slot = kInnEquipmentSlots[i];
                const Rectangle rect{slotStartX + static_cast<float>(col) * (slotW + slotGapX), slotStartY + static_cast<float>(row) * (slotH + slotGapY), slotW, slotH};
                slotRects[i] = rect;

                Color bg = Color{42, 48, 64, 220};
                if (innDraggedItem.has_value() && items::CanEquipInSlot(*innDraggedItem, slot)) {
                    bg = Color{88, 124, 72, 230};
                }

                DrawRectangleRec(rect, bg);
                DrawRectangleLinesEx(rect, 2.0f, Color{120, 140, 175, 255});
                DrawText(items::EquipmentSlotToString(slot), static_cast<int>(rect.x + 4.0f * sx), static_cast<int>(rect.y + 4.0f * sy), static_cast<int>(12.0f * sf), Color{190, 205, 220, 255});

                const std::optional<items::ItemId> equipped = innEquipmentLoadout.GetEquippedItem(slot);
                if (equipped.has_value()) {
                    const items::ItemDefinition& def = items::GetItemDefinition(*equipped);
                    DrawText(def.name, static_cast<int>(rect.x + 4.0f * sx), static_cast<int>(rect.y + 24.0f * sy), static_cast<int>(13.0f * sf), Color{240, 212, 120, 255});
                }
            }

            int bonusHp = 0;
            int bonusAttackMin = 0;
            int bonusAttackMax = 0;
            int bonusHealMin = 0;
            int bonusHealMax = 0;
            int equippedCount = 0;
            for (const items::EquipmentSlot slot : kInnEquipmentSlots) {
                const std::optional<items::ItemId> equipped = innEquipmentLoadout.GetEquippedItem(slot);
                if (!equipped.has_value()) {
                    continue;
                }

                equippedCount++;
                const items::ItemDefinition::StatBonuses bonuses = items::GetItemDefinition(*equipped).statBonuses;
                bonusHp += bonuses.maxHp;
                bonusAttackMin += bonuses.attackMin;
                bonusAttackMax += bonuses.attackMax;
                bonusHealMin += bonuses.healMin;
                bonusHealMax += bonuses.healMax;
            }

            const Rectangle bonusPanel{leftX + 8.0f * sx, 414.0f * sy, panelW - 16.0f * sx, 64.0f * sy};
            DrawRectangleRec(bonusPanel, Color{24, 30, 44, 230});
            DrawRectangleLinesEx(bonusPanel, 1.5f, Color{108, 128, 160, 255});
            DrawText(TextFormat("Total Equipment Bonus (%d equipped)", equippedCount),
                     static_cast<int>(bonusPanel.x + 6.0f * sx),
                     static_cast<int>(bonusPanel.y + 4.0f * sy),
                     static_cast<int>(15.0f * sf),
                     RAYWHITE);
            DrawText(TextFormat("HP: +%d", bonusHp),
                     static_cast<int>(bonusPanel.x + 6.0f * sx),
                     static_cast<int>(bonusPanel.y + 24.0f * sy),
                     static_cast<int>(14.0f * sf),
                     Color{190, 205, 220, 255});
            DrawText(TextFormat("ATK: +%d to +%d", bonusAttackMin, bonusAttackMax),
                     static_cast<int>(bonusPanel.x + 122.0f * sx),
                     static_cast<int>(bonusPanel.y + 24.0f * sy),
                     static_cast<int>(14.0f * sf),
                     Color{190, 205, 220, 255});
            DrawText(TextFormat("HEAL: +%d to +%d", bonusHealMin, bonusHealMax),
                     static_cast<int>(bonusPanel.x + 6.0f * sx),
                     static_cast<int>(bonusPanel.y + 43.0f * sy),
                     static_cast<int>(14.0f * sf),
                     Color{190, 205, 220, 255});

            if (innInventoryEntries.empty()) {
                DrawText("Inventory is empty.", static_cast<int>(rightX + 12.0f * sx), static_cast<int>(136.0f * sy), static_cast<int>(16.0f * sf), Color{190, 205, 220, 255});
            } else {
                const int rows = std::min(11, static_cast<int>(innInventoryEntries.size()));
                for (int i = 0; i < rows; ++i) {
                    const items::InventoryEntry& entry = innInventoryEntries[static_cast<size_t>(i)];
                    const items::ItemDefinition& def = items::GetItemDefinition(entry.id);
                    const Rectangle rowRect{rightX + 10.0f * sx, (128.0f + static_cast<float>(i) * 32.0f) * sy, panelW - 20.0f * sx, 28.0f * sy};

                    const bool isDragged = innDraggedItem.has_value() && *innDraggedItem == entry.id;
                    DrawRectangleRec(rowRect, isDragged ? Color{74, 98, 130, 230} : Color{42, 48, 64, 220});
                    DrawRectangleLinesEx(rowRect, 1.0f, Color{96, 112, 140, 255});
                    DrawText(TextFormat("%s x%d", def.name, entry.quantity), static_cast<int>(rowRect.x + 6.0f * sx), static_cast<int>(rowRect.y + 6.0f * sy), static_cast<int>(14.0f * sf), Color{190, 205, 220, 255});
                }
            }

            if (innDraggedItem.has_value()) {
                const Vector2 mouse = GetMousePosition();
                const items::ItemDefinition& dragDef = items::GetItemDefinition(*innDraggedItem);
                DrawRectangle(static_cast<int>(mouse.x + 8.0f), static_cast<int>(mouse.y + 8.0f), static_cast<int>(170.0f * sx), static_cast<int>(24.0f * sy), Color{20, 24, 32, 220});
                DrawText(dragDef.name, static_cast<int>(mouse.x + 12.0f), static_cast<int>(mouse.y + 12.0f), static_cast<int>(14.0f * sf), Color{240, 212, 120, 255});
            }

            DrawText("LMB drag from inventory to equip. RMB on slot to unequip.", static_cast<int>(leftX), static_cast<int>(486.0f * sy), static_cast<int>(16.0f * sf), Color{190, 205, 220, 255});
        }

        // Draw inn action buttons last so they stay above the panel layer.
        if (!innEquipmentView) {
            ui::DrawButton(innSaveButton);
        }
        ui::DrawButton(equipmentButton);
    }

    if (shopOpen) {
        const std::vector<items::ItemId>& shopItems = items::GetShopItemIds();
        const int totalShopItems = static_cast<int>(shopItems.size());
        const int shownCount = std::max(0, std::min(static_cast<int>(shopVisibleButtons.size()), totalShopItems - shopScrollOffset));

        DrawText("Shop", static_cast<int>(50.0f * sx), static_cast<int>(92.0f * sy), static_cast<int>(42.0f * sf), RAYWHITE);
        DrawRectangle(static_cast<int>(50.0f * sx), static_cast<int>(150.0f * sy), static_cast<int>(450.0f * sx), static_cast<int>(320.0f * sy), Color{28, 34, 50, 200});
        DrawRectangleLines(static_cast<int>(50.0f * sx), static_cast<int>(150.0f * sy), static_cast<int>(450.0f * sx), static_cast<int>(320.0f * sy), Color{96, 112, 140, 255});

        ui::DrawButton(shopScrollUpButton);
        ui::DrawButton(shopScrollDownButton);
        for (int i = 0; i < shownCount; ++i) {
            ui::DrawButton(shopVisibleButtons[static_cast<size_t>(i)]);
        }

        const int firstShown = totalShopItems > 0 ? std::min(shopScrollOffset + 1, totalShopItems) : 0;
        const int lastShown = totalShopItems > 0 ? std::min(shopScrollOffset + shownCount, totalShopItems) : 0;
        DrawText("Mouse wheel or arrows to scroll", static_cast<int>(64.0f * sx), static_cast<int>(448.0f * sy), static_cast<int>(16.0f * sf), Color{190, 205, 220, 255});
        DrawText(TextFormat("Offers %d-%d / %d", firstShown, lastShown, totalShopItems), static_cast<int>(312.0f * sx), static_cast<int>(448.0f * sy), static_cast<int>(16.0f * sf), Color{190, 205, 220, 255});

        DrawRectangle(static_cast<int>(530.0f * sx), static_cast<int>(150.0f * sy), static_cast<int>(380.0f * sx), static_cast<int>(320.0f * sy), Color{28, 34, 50, 200});
        DrawRectangleLines(static_cast<int>(530.0f * sx), static_cast<int>(150.0f * sy), static_cast<int>(380.0f * sx), static_cast<int>(320.0f * sy), Color{96, 112, 140, 255});
        DrawText("Item Stats", static_cast<int>(544.0f * sx), static_cast<int>(162.0f * sy), static_cast<int>(24.0f * sf), RAYWHITE);

        if (hoveredShopItem.has_value()) {
            const items::ItemDefinition& definition = items::GetItemDefinition(*hoveredShopItem);
            DrawText(definition.name, static_cast<int>(544.0f * sx), static_cast<int>(198.0f * sy), static_cast<int>(24.0f * sf), Color{240, 212, 120, 255});
            DrawText(TextFormat("Category: %s", ItemCategoryToString(definition.category)), static_cast<int>(544.0f * sx), static_cast<int>(230.0f * sy), static_cast<int>(18.0f * sf), Color{195, 205, 220, 255});
            DrawText(TextFormat("Buy Price: %dg", definition.buyPrice), static_cast<int>(544.0f * sx), static_cast<int>(254.0f * sy), static_cast<int>(18.0f * sf), Color{195, 205, 220, 255});
            DrawText(TextFormat("Max Stack: %d", definition.maxStack), static_cast<int>(544.0f * sx), static_cast<int>(278.0f * sy), static_cast<int>(18.0f * sf), Color{195, 205, 220, 255});
            DrawText("Description:", static_cast<int>(544.0f * sx), static_cast<int>(312.0f * sy), static_cast<int>(18.0f * sf), Color{205, 210, 220, 255});
            DrawText(definition.description, static_cast<int>(544.0f * sx), static_cast<int>(334.0f * sy), static_cast<int>(18.0f * sf), Color{185, 195, 210, 255});
        } else {
            DrawText("Hover an offer to inspect", static_cast<int>(544.0f * sx), static_cast<int>(206.0f * sy), static_cast<int>(18.0f * sf), Color{190, 205, 220, 255});
            DrawText("item details and stats.", static_cast<int>(544.0f * sx), static_cast<int>(230.0f * sy), static_cast<int>(18.0f * sf), Color{190, 205, 220, 255});
        }
    }

    DrawText(infoText.c_str(), static_cast<int>(40.0f * sx), static_cast<int>(498.0f * sy), static_cast<int>(18.0f * sf), Color{205, 210, 220, 255});
}
