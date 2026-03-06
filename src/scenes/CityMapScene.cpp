#include "scenes/CityMapScene.h"

#include <algorithm>

#include "raylib.h"

CityMapScene::CityMapScene()
    : tavernButton{Rectangle{120.0f, 150.0f, 220.0f, 64.0f}, "Tavern", true},
      guildButton{Rectangle{360.0f, 150.0f, 220.0f, 64.0f}, "Guild", true},
      shopButton{Rectangle{120.0f, 250.0f, 220.0f, 64.0f}, "Shop", true},
      innButton{Rectangle{360.0f, 250.0f, 220.0f, 64.0f}, "Inn", true},
      buyPotionButton{Rectangle{620.0f, 146.0f, 300.0f, 44.0f}, "Buy Potion", true},
      buyGreaterPotionButton{Rectangle{620.0f, 196.0f, 300.0f, 44.0f}, "Buy Greater Potion", true},
      buyElixirButton{Rectangle{620.0f, 246.0f, 300.0f, 44.0f}, "Buy Elixir", true},
      buySwordButton{Rectangle{620.0f, 296.0f, 300.0f, 44.0f}, "Buy Bronze Sword", true},
      buyArmorButton{Rectangle{620.0f, 346.0f, 300.0f, 44.0f}, "Buy Armor", true},
      buyAmuletButton{Rectangle{620.0f, 396.0f, 300.0f, 44.0f}, "Buy Focus Amulet", true},
      worldButton{Rectangle{700.0f, 440.0f, 220.0f, 56.0f}, "Back To World", true},
      playerGold(0),
      requestedShopItem(std::nullopt),
      infoText("Choose a place in the city.") {}

void CityMapScene::SetPlayerGold(int value) {
    playerGold = std::max(0, value);
}

void CityMapScene::SetShopFeedback(const std::string& value) {
    if (!value.empty()) {
        infoText = value;
    }
}

std::optional<items::ItemId> CityMapScene::ConsumeRequestedShopItem() {
    std::optional<items::ItemId> result = requestedShopItem;
    requestedShopItem = std::nullopt;
    return result;
}

CityMapAction CityMapScene::Update() {
    const float sx = static_cast<float>(GetScreenWidth()) / 960.0f;
    const float sy = static_cast<float>(GetScreenHeight()) / 540.0f;

    tavernButton.bounds = Rectangle{120.0f * sx, 150.0f * sy, 220.0f * sx, 64.0f * sy};
    guildButton.bounds = Rectangle{360.0f * sx, 150.0f * sy, 220.0f * sx, 64.0f * sy};
    shopButton.bounds = Rectangle{120.0f * sx, 250.0f * sy, 220.0f * sx, 64.0f * sy};
    innButton.bounds = Rectangle{360.0f * sx, 250.0f * sy, 220.0f * sx, 64.0f * sy};
    buyPotionButton.bounds = Rectangle{620.0f * sx, 146.0f * sy, 300.0f * sx, 44.0f * sy};
    buyGreaterPotionButton.bounds = Rectangle{620.0f * sx, 196.0f * sy, 300.0f * sx, 44.0f * sy};
    buyElixirButton.bounds = Rectangle{620.0f * sx, 246.0f * sy, 300.0f * sx, 44.0f * sy};
    buySwordButton.bounds = Rectangle{620.0f * sx, 296.0f * sy, 300.0f * sx, 44.0f * sy};
    buyArmorButton.bounds = Rectangle{620.0f * sx, 346.0f * sy, 300.0f * sx, 44.0f * sy};
    buyAmuletButton.bounds = Rectangle{620.0f * sx, 396.0f * sy, 300.0f * sx, 44.0f * sy};
    worldButton.bounds = Rectangle{700.0f * sx, 440.0f * sy, 220.0f * sx, 56.0f * sy};

    if (ui::IsPressed(tavernButton)) {
        infoText = "Tavern: Rumors of monsters in the deep dungeon.";
    } else if (ui::IsPressed(guildButton)) {
        infoText = "Guild: Contracts and training will be available here.";
    } else if (ui::IsPressed(shopButton)) {
        infoText = "Shop: Supplies and equipment can be bought here.";
    } else if (ui::IsPressed(innButton)) {
        infoText = "Inn: Rest complete. You can save your progress.";
        return CityMapAction::SaveAtInn;
    } else if (ui::IsPressed(buyPotionButton)) {
        requestedShopItem = items::ItemId::HealthPotion;
        return CityMapAction::BuyItem;
    } else if (ui::IsPressed(buyGreaterPotionButton)) {
        requestedShopItem = items::ItemId::GreaterHealthPotion;
        return CityMapAction::BuyItem;
    } else if (ui::IsPressed(buyElixirButton)) {
        requestedShopItem = items::ItemId::Elixir;
        return CityMapAction::BuyItem;
    } else if (ui::IsPressed(buySwordButton)) {
        requestedShopItem = items::ItemId::BronzeSword;
        return CityMapAction::BuyItem;
    } else if (ui::IsPressed(buyArmorButton)) {
        requestedShopItem = items::ItemId::ReinforcedArmor;
        return CityMapAction::BuyItem;
    } else if (ui::IsPressed(buyAmuletButton)) {
        requestedShopItem = items::ItemId::FocusAmulet;
        return CityMapAction::BuyItem;
    } else if (ui::IsPressed(worldButton)) {
        return CityMapAction::BackToWorld;
    }

    return CityMapAction::None;
}

void CityMapScene::Draw() const {
    const float sx = static_cast<float>(GetScreenWidth()) / 960.0f;
    const float sy = static_cast<float>(GetScreenHeight()) / 540.0f;
    const float sf = std::min(sx, sy);

    DrawText("City Map", static_cast<int>(36.0f * sx), static_cast<int>(30.0f * sy), static_cast<int>(50.0f * sf), RAYWHITE);
    DrawText("Districts", static_cast<int>(42.0f * sx), static_cast<int>(88.0f * sy), static_cast<int>(24.0f * sf), LIGHTGRAY);
    DrawText(TextFormat("Gold: %d", playerGold), static_cast<int>(620.0f * sx), static_cast<int>(92.0f * sy), static_cast<int>(24.0f * sf), Color{240, 212, 120, 255});

    DrawRectangle(static_cast<int>(90.0f * sx), static_cast<int>(130.0f * sy), static_cast<int>(520.0f * sx), static_cast<int>(220.0f * sy), Color{28, 34, 50, 190});
    DrawRectangleLines(static_cast<int>(90.0f * sx), static_cast<int>(130.0f * sy), static_cast<int>(520.0f * sx), static_cast<int>(220.0f * sy), Color{120, 140, 175, 255});

    ui::DrawButton(tavernButton);
    ui::DrawButton(guildButton);
    ui::DrawButton(shopButton);
    ui::DrawButton(innButton);
    ui::DrawButton(buyPotionButton);
    ui::DrawButton(buyGreaterPotionButton);
    ui::DrawButton(buyElixirButton);
    ui::DrawButton(buySwordButton);
    ui::DrawButton(buyArmorButton);
    ui::DrawButton(buyAmuletButton);
    ui::DrawButton(worldButton);

    DrawText("Shop", static_cast<int>(620.0f * sx), static_cast<int>(122.0f * sy), static_cast<int>(24.0f * sf), RAYWHITE);
    DrawText("Potion 20g | Greater 45g | Elixir 95g", static_cast<int>(622.0f * sx), static_cast<int>(432.0f * sy), static_cast<int>(16.0f * sf), Color{190, 205, 220, 255});
    DrawText("Sword 45g | Armor 90g | Amulet 70g", static_cast<int>(622.0f * sx), static_cast<int>(450.0f * sy), static_cast<int>(16.0f * sf), Color{190, 205, 220, 255});

    DrawText(infoText.c_str(), static_cast<int>(40.0f * sx), static_cast<int>(498.0f * sy), static_cast<int>(18.0f * sf), Color{205, 210, 220, 255});
}
