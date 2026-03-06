#pragma once

#include <optional>
#include <string>

#include "items/ItemSystem.h"
#include "ui/Button.h"

enum class CityMapAction {
    None,
    BuyItem,
    SaveAtInn,
    BackToWorld
};

class CityMapScene {
public:
    CityMapScene();

    void SetPlayerGold(int value);
    void SetShopFeedback(const std::string& value);
    std::optional<items::ItemId> ConsumeRequestedShopItem();

    CityMapAction Update();
    void Draw() const;

private:
    ui::Button tavernButton;
    ui::Button guildButton;
    ui::Button shopButton;
    ui::Button innButton;
    ui::Button buyPotionButton;
    ui::Button buyGreaterPotionButton;
    ui::Button buyElixirButton;
    ui::Button buySwordButton;
    ui::Button buyArmorButton;
    ui::Button buyAmuletButton;
    ui::Button worldButton;
    int playerGold;
    std::optional<items::ItemId> requestedShopItem;
    std::string infoText;
};
