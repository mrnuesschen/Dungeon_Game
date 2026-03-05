#pragma once

#include "ui/Button.h"

enum class MainMenuAction {
    None,
    NewGame,
    LoadGame,
    OpenGallery,
    QuitGame
};

class MainMenuScene {
public:
    MainMenuScene();

    MainMenuAction Update(bool canLoadGame);
    void Draw(bool canLoadGame) const;

private:
    ui::Button newGameButton;
    ui::Button loadGameButton;
    ui::Button galleryButton;
    ui::Button quitButton;
};
