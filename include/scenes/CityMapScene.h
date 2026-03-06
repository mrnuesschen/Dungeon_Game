#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>

#include "GameTypes.h"
#include "items/EquipmentLoadout.h"
#include "items/Inventory.h"
#include "items/ItemSystem.h"
#include "quests/QuestSystem.h"
#include "ui/Button.h"

enum class CityMapAction {
    None,
    BuyItem,
    EquipItemAtInn,
    UnequipItemAtInn,
    SaveAtInn,
    BackToWorld
};

struct CityMapEquipRequest {
    items::ItemId itemId;
    items::EquipmentSlot slot;
};

class CityMapScene {
public:
    CityMapScene();

    void SetPlayerGold(int value);
    void SetInnSnapshot(const BattleSaveData& saveData);
    void SetInnEquipmentSnapshot(const items::EquipmentLoadout& loadout);
    void SetShopFeedback(const std::string& value);
    void RegisterBattleVictoryForQuests(int floor, const std::vector<EnemyArchetype>& defeatedEnemies);
    void RegisterQuestItemCollected(quests::QuestId questId, items::ItemId itemId, int floor);
    std::vector<quests::QuestItemSpawn> GetPendingQuestItemSpawnsForFloor(int floor) const;
    std::optional<items::ItemId> ConsumeRequestedShopItem();
    std::optional<CityMapEquipRequest> ConsumeRequestedEquipItem();
    std::optional<items::EquipmentSlot> ConsumeRequestedUnequipSlot();

    CityMapAction Update();
    void Draw() const;

private:
    ui::Button tavernButton;
    ui::Button guildButton;
    ui::Button shopButton;
    ui::Button innButton;
    ui::Button worldButton;

    ui::Button closeBuildingButton;
    ui::Button innSaveButton;
    ui::Button innEquipmentButton;
    ui::Button guildViewToggleButton;
    ui::Button guildOfferScrollUpButton;
    ui::Button guildOfferScrollDownButton;
    std::array<ui::Button, 2> guildAcceptButtons;
    ui::Button shopScrollUpButton;
    ui::Button shopScrollDownButton;
    std::array<ui::Button, 5> shopVisibleButtons;

    bool tavernOpen;
    bool shopOpen;
    bool guildOpen;
    bool guildShowAcceptedList;
    bool innOpen;
    bool innEquipmentView;
    int guildOfferScrollOffset;
    int shopScrollOffset;

    std::string innPlayerName;
    PlayerClass innPlayerClass;
    int innPlayerLevel;
    int innPlayerHp;
    std::vector<items::InventoryEntry> innInventoryEntries;
    items::EquipmentLoadout innEquipmentLoadout;
    std::optional<items::ItemId> innDraggedItem;
    std::vector<quests::QuestDefinition> displayedGuildQuests;
    quests::QuestLog questLog;

    int playerGold;
    std::optional<items::ItemId> requestedShopItem;
    std::optional<CityMapEquipRequest> requestedEquipItem;
    std::optional<items::EquipmentSlot> requestedUnequipSlot;
    std::optional<items::ItemId> hoveredShopItem;
    std::string infoText;

    void RefreshGuildQuestSelection();
};
