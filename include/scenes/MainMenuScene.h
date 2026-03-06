#pragma once

#include <array>
#include <vector>

#include "GameTypes.h"
#include "ui/Button.h"

enum class MainMenuAction {
    None,
    NewGame,
    LoadGame,
    DeleteSave,
    OpenGallery,
    QuitGame
};

class MainMenuScene {
public:
    MainMenuScene();

    MainMenuAction Update(const std::vector<SaveSlotPreview>& saveSlots);
    void Draw(const std::vector<SaveSlotPreview>& saveSlots) const;
    int ConsumeSelectedSaveSlot();

private:
    static constexpr int kVisibleSaveRows = 6;

    enum class ViewMode {
        Root,
        SaveList
    };

    ViewMode viewMode;
    int selectedSaveSlot;

    ui::Button newGameButton;
    ui::Button loadGameButton;
    ui::Button galleryButton;
    ui::Button quitButton;
    ui::Button saveListBackButton;
    std::array<ui::Button, kVisibleSaveRows> loadSlotButtons;
    std::array<ui::Button, kVisibleSaveRows> deleteSlotButtons;
};
