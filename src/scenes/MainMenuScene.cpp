#include "scenes/MainMenuScene.h"

#include <algorithm>

#include "raylib.h"

MainMenuScene::MainMenuScene()
        : viewMode(ViewMode::Root),
          selectedSaveSlot(0),
          newGameButton{Rectangle{330.0f, 170.0f, 300.0f, 56.0f}, "New Game", true},
          loadGameButton{Rectangle{330.0f, 240.0f, 300.0f, 56.0f}, "Load Game", true},
          galleryButton{Rectangle{330.0f, 310.0f, 300.0f, 56.0f}, "Gallery", true},
          quitButton{Rectangle{330.0f, 380.0f, 300.0f, 56.0f}, "Quit", true},
          saveListBackButton{Rectangle{740.0f, 462.0f, 180.0f, 52.0f}, "Back", true} {
    for (int i = 0; i < kVisibleSaveRows; ++i) {
        loadSlotButtons[static_cast<size_t>(i)] = ui::Button{Rectangle{80.0f, 142.0f + i * 54.0f, 620.0f, 44.0f}, "Load", true};
        deleteSlotButtons[static_cast<size_t>(i)] = ui::Button{Rectangle{716.0f, 142.0f + i * 54.0f, 160.0f, 44.0f}, "Delete", true};
    }
}

int MainMenuScene::ConsumeSelectedSaveSlot() {
    const int value = selectedSaveSlot;
    selectedSaveSlot = 0;
    return value;
}

MainMenuAction MainMenuScene::Update(const std::vector<SaveSlotPreview>& saveSlots) {
    const float sx = static_cast<float>(GetScreenWidth()) / 960.0f;
    const float sy = static_cast<float>(GetScreenHeight()) / 540.0f;

    if (viewMode == ViewMode::Root) {
        const float bw = 300.0f * sx;
        const float bh = 56.0f * sy;
        const float x = (static_cast<float>(GetScreenWidth()) - bw) * 0.5f;

        newGameButton.bounds = Rectangle{x, 170.0f * sy, bw, bh};
        loadGameButton.bounds = Rectangle{x, 240.0f * sy, bw, bh};
        galleryButton.bounds = Rectangle{x, 310.0f * sy, bw, bh};
        quitButton.bounds = Rectangle{x, 380.0f * sy, bw, bh};

        loadGameButton.enabled = !saveSlots.empty();

        if (ui::IsPressed(newGameButton)) {
            return MainMenuAction::NewGame;
        }
        if (ui::IsPressed(loadGameButton)) {
            viewMode = ViewMode::SaveList;
            return MainMenuAction::None;
        }
        if (ui::IsPressed(galleryButton)) {
            return MainMenuAction::OpenGallery;
        }
        if (ui::IsPressed(quitButton)) {
            return MainMenuAction::QuitGame;
        }

        return MainMenuAction::None;
    }

    const float listX = 72.0f * sx;
    const float rowY = 136.0f * sy;
    const float rowH = 52.0f * sy;
    const float rowGap = 8.0f * sy;
    const float loadW = 640.0f * sx;
    const float deleteX = listX + loadW + 10.0f * sx;
    const float deleteW = 166.0f * sx;

    for (int i = 0; i < kVisibleSaveRows; ++i) {
        const float y = rowY + static_cast<float>(i) * (rowH + rowGap);
        loadSlotButtons[static_cast<size_t>(i)].bounds = Rectangle{listX, y, loadW, rowH};
        deleteSlotButtons[static_cast<size_t>(i)].bounds = Rectangle{deleteX, y, deleteW, rowH};
    }

    saveListBackButton.bounds = Rectangle{740.0f * sx, 462.0f * sy, 180.0f * sx, 52.0f * sy};

    const int visibleSlots = std::min(static_cast<int>(saveSlots.size()), kVisibleSaveRows);
    for (int i = 0; i < visibleSlots; ++i) {
        loadSlotButtons[static_cast<size_t>(i)].enabled = true;
        deleteSlotButtons[static_cast<size_t>(i)].enabled = true;
        loadSlotButtons[static_cast<size_t>(i)].text = saveSlots[static_cast<size_t>(i)].label;

        if (ui::IsPressed(loadSlotButtons[static_cast<size_t>(i)])) {
            selectedSaveSlot = saveSlots[static_cast<size_t>(i)].slotIndex;
            return MainMenuAction::LoadGame;
        }

        if (ui::IsPressed(deleteSlotButtons[static_cast<size_t>(i)])) {
            selectedSaveSlot = saveSlots[static_cast<size_t>(i)].slotIndex;
            return MainMenuAction::DeleteSave;
        }
    }

    if (ui::IsPressed(saveListBackButton) || IsKeyPressed(KEY_ESCAPE)) {
        viewMode = ViewMode::Root;
    }

    return MainMenuAction::None;
}

void MainMenuScene::Draw(const std::vector<SaveSlotPreview>& saveSlots) const {
    const float sx = static_cast<float>(GetScreenWidth()) / 960.0f;
    const float sy = static_cast<float>(GetScreenHeight()) / 540.0f;
    const float sf = std::min(sx, sy);

    if (viewMode == ViewMode::Root) {
        DrawText("Dungeon Game", static_cast<int>(330.0f * sx), static_cast<int>(70.0f * sy), static_cast<int>(54.0f * sf), RAYWHITE);
        DrawText("Main Menu", static_cast<int>(388.0f * sx), static_cast<int>(128.0f * sy), static_cast<int>(30.0f * sf), LIGHTGRAY);

        ui::Button loadButton = loadGameButton;
        loadButton.enabled = !saveSlots.empty();

        ui::DrawButton(newGameButton);
        ui::DrawButton(loadButton);
        ui::DrawButton(galleryButton);
        ui::DrawButton(quitButton);

        if (saveSlots.empty()) {
            DrawText("No save games found", static_cast<int>(338.0f * sx), static_cast<int>(458.0f * sy), static_cast<int>(24.0f * sf), GRAY);
        }
        return;
    }

    DrawText("Load Game", static_cast<int>(346.0f * sx), static_cast<int>(58.0f * sy), static_cast<int>(50.0f * sf), RAYWHITE);
    DrawText("Choose a slot to load or delete", static_cast<int>(296.0f * sx), static_cast<int>(104.0f * sy), static_cast<int>(24.0f * sf), LIGHTGRAY);

    const int visibleSlots = std::min(static_cast<int>(saveSlots.size()), kVisibleSaveRows);
    for (int i = 0; i < visibleSlots; ++i) {
        ui::DrawButton(loadSlotButtons[static_cast<size_t>(i)]);
        ui::DrawButton(deleteSlotButtons[static_cast<size_t>(i)]);
    }

    if (visibleSlots == 0) {
        DrawText("No save slots available.", static_cast<int>(322.0f * sx), static_cast<int>(230.0f * sy), static_cast<int>(26.0f * sf), GRAY);
    }

    ui::DrawButton(saveListBackButton);
}
