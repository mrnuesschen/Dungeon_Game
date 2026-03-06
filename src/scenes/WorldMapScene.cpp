#include "scenes/WorldMapScene.h"

#include <algorithm>

#include "raylib.h"

WorldMapScene::WorldMapScene()
    : playerName("Hero"),
      dungeonButton{Rectangle{120.0f, 260.0f, 260.0f, 64.0f}, "Dungeon", true},
      cityButton{Rectangle{420.0f, 180.0f, 260.0f, 64.0f}, "City", true},
      menuButton{Rectangle{760.0f, 450.0f, 160.0f, 56.0f}, "Main Menu", true} {}

void WorldMapScene::Enter(const std::string& value) {
    if (value.empty()) {
        playerName = "Hero";
        return;
    }

    playerName = value;
}

WorldMapAction WorldMapScene::Update() {
    const float sx = static_cast<float>(GetScreenWidth()) / 960.0f;
    const float sy = static_cast<float>(GetScreenHeight()) / 540.0f;

    dungeonButton.bounds = Rectangle{120.0f * sx, 260.0f * sy, 260.0f * sx, 64.0f * sy};
    cityButton.bounds = Rectangle{420.0f * sx, 180.0f * sy, 260.0f * sx, 64.0f * sy};
    menuButton.bounds = Rectangle{760.0f * sx, 450.0f * sy, 160.0f * sx, 56.0f * sy};

    if (ui::IsPressed(dungeonButton)) {
        return WorldMapAction::EnterDungeon;
    }
    if (ui::IsPressed(cityButton)) {
        return WorldMapAction::EnterCity;
    }
    if (ui::IsPressed(menuButton)) {
        return WorldMapAction::BackToMenu;
    }

    return WorldMapAction::None;
}

void WorldMapScene::Draw() const {
    const float sx = static_cast<float>(GetScreenWidth()) / 960.0f;
    const float sy = static_cast<float>(GetScreenHeight()) / 540.0f;
    const float sf = std::min(sx, sy);

    DrawText("World Map", static_cast<int>(36.0f * sx), static_cast<int>(30.0f * sy), static_cast<int>(50.0f * sf), RAYWHITE);
    DrawText(TextFormat("Traveler: %s", playerName.c_str()), static_cast<int>(38.0f * sx), static_cast<int>(84.0f * sy), static_cast<int>(24.0f * sf), LIGHTGRAY);

    DrawLineEx(Vector2{250.0f * sx, 292.0f * sy}, Vector2{548.0f * sx, 212.0f * sy}, 4.0f * sf, Color{150, 130, 100, 255});

    DrawCircle(static_cast<int>(250.0f * sx), static_cast<int>(292.0f * sy), 56.0f * sf, Color{95, 74, 58, 255});
    DrawCircle(static_cast<int>(548.0f * sx), static_cast<int>(212.0f * sy), 56.0f * sf, Color{65, 95, 120, 255});

    ui::DrawButton(dungeonButton);
    ui::DrawButton(cityButton);
    ui::DrawButton(menuButton);

    DrawText("Enter the dungeon to fight enemy groups.", static_cast<int>(40.0f * sx), static_cast<int>(500.0f * sy), static_cast<int>(18.0f * sf), Color{190, 196, 210, 255});
}
