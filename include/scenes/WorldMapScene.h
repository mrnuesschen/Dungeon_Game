#pragma once

#include <string>

#include "ui/Button.h"

enum class WorldMapAction {
    None,
    EnterDungeon,
    EnterCity,
    BackToMenu
};

class WorldMapScene {
public:
    WorldMapScene();

    void Enter(const std::string& playerName);
    WorldMapAction Update();
    void Draw() const;

private:
    std::string playerName;
    ui::Button dungeonButton;
    ui::Button cityButton;
    ui::Button menuButton;
};
