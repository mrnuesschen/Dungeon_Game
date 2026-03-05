#include "scenes/MainMenuScene.h"

#include <algorithm>

#include "raylib.h"

MainMenuScene::MainMenuScene()
        : newGameButton{Rectangle{330.0f, 170.0f, 300.0f, 56.0f}, "New Game", true},
            loadGameButton{Rectangle{330.0f, 240.0f, 300.0f, 56.0f}, "Load Game", true},
            galleryButton{Rectangle{330.0f, 310.0f, 300.0f, 56.0f}, "Gallery", true},
            quitButton{Rectangle{330.0f, 380.0f, 300.0f, 56.0f}, "Quit", true} {}

MainMenuAction MainMenuScene::Update(bool canLoadGame) {
    const float sx = static_cast<float>(GetScreenWidth()) / 960.0f;
    const float sy = static_cast<float>(GetScreenHeight()) / 540.0f;

    const float bw = 300.0f * sx;
    const float bh = 56.0f * sy;
    const float x = (static_cast<float>(GetScreenWidth()) - bw) * 0.5f;

    newGameButton.bounds = Rectangle{x, 170.0f * sy, bw, bh};
    loadGameButton.bounds = Rectangle{x, 240.0f * sy, bw, bh};
    galleryButton.bounds = Rectangle{x, 310.0f * sy, bw, bh};
    quitButton.bounds = Rectangle{x, 380.0f * sy, bw, bh};

    loadGameButton.enabled = canLoadGame;

    if (ui::IsPressed(newGameButton)) {
        return MainMenuAction::NewGame;
    }
    if (ui::IsPressed(loadGameButton)) {
        return MainMenuAction::LoadGame;
    }
    if (ui::IsPressed(galleryButton)) {
        return MainMenuAction::OpenGallery;
    }
    if (ui::IsPressed(quitButton)) {
        return MainMenuAction::QuitGame;
    }

    return MainMenuAction::None;
}

void MainMenuScene::Draw(bool canLoadGame) const {
    const float sx = static_cast<float>(GetScreenWidth()) / 960.0f;
    const float sy = static_cast<float>(GetScreenHeight()) / 540.0f;
    const float sf = std::min(sx, sy);

    DrawText("Dungeon Game", static_cast<int>(330.0f * sx), static_cast<int>(70.0f * sy), static_cast<int>(54.0f * sf), RAYWHITE);
    DrawText("Main Menu", static_cast<int>(388.0f * sx), static_cast<int>(128.0f * sy), static_cast<int>(30.0f * sf), LIGHTGRAY);

    ui::Button loadButton = loadGameButton;
    loadButton.enabled = canLoadGame;

    ui::DrawButton(newGameButton);
    ui::DrawButton(loadButton);
    ui::DrawButton(galleryButton);
    ui::DrawButton(quitButton);

    if (!canLoadGame) {
        DrawText("No save game found", static_cast<int>(335.0f * sx), static_cast<int>(458.0f * sy), static_cast<int>(24.0f * sf), GRAY);
    }
}
