#pragma once

#include <string>
#include <vector>

#include "GameTypes.h"
#include "ui/Button.h"

enum class CharacterCreationAction {
    None,
    Back,
    Start
};

class CharacterCreationScene {
public:
    CharacterCreationScene();
    ~CharacterCreationScene();

    void Enter();
    CharacterCreationAction Update();
    void Draw() const;

    CharacterSetupData GetSetupData() const;

private:
    void CollectAvatarFiles();
    void LoadPreviewTexture();
    void UnloadPreviewTexture();
    void CycleClass();
    void NextAvatar();
    void PreviousAvatar();

    CharacterSetupData setup;
    std::vector<std::string> avatarPaths;
    int avatarIndex;

    ui::Button classButton;
    ui::Button prevAvatarButton;
    ui::Button nextAvatarButton;
    ui::Button startButton;
    ui::Button backButton;

    Texture2D avatarPreview;
    bool avatarPreviewLoaded;
};
