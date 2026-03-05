#include "scenes/CharacterCreationScene.h"

#include <algorithm>
#include <cctype>
#include <filesystem>

#include "entities/Player.h"
#include "raylib.h"

namespace {

PlayerClass NextClass(PlayerClass value) {
    switch (value) {
    case PlayerClass::Tank:
        return PlayerClass::Knight;
    case PlayerClass::Knight:
        return PlayerClass::Assassin;
    case PlayerClass::Assassin:
        return PlayerClass::Mage;
    case PlayerClass::Mage:
        return PlayerClass::Healer;
    case PlayerClass::Healer:
        return PlayerClass::Alchemist;
    case PlayerClass::Alchemist:
        return PlayerClass::Tank;
    }

    return PlayerClass::Knight;
}

bool HasImageExtension(const std::filesystem::path& path) {
    const std::string ext = path.extension().string();
    std::string lower;
    lower.resize(ext.size());
    std::transform(ext.begin(), ext.end(), lower.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });

    return lower == ".png" || lower == ".jpg" || lower == ".jpeg" || lower == ".bmp";
}

std::filesystem::path ResolveAvatarDirectory() {
    std::filesystem::path current = std::filesystem::current_path();

    for (int i = 0; i < 8; ++i) {
        const std::filesystem::path candidate = current / "assets" / "sprites" / "player";
        if (std::filesystem::exists(candidate) && std::filesystem::is_directory(candidate)) {
            return candidate;
        }

        if (!current.has_parent_path()) {
            break;
        }

        const std::filesystem::path parent = current.parent_path();
        if (parent == current) {
            break;
        }

        current = parent;
    }

    return {};
}

}

CharacterCreationScene::CharacterCreationScene()
    : setup(),
      avatarIndex(0),
            classButton{Rectangle{80.0f, 120.0f, 360.0f, 56.0f}, "Class", true},
      prevAvatarButton{Rectangle{80.0f, 360.0f, 170.0f, 56.0f}, "< Avatar", true},
      nextAvatarButton{Rectangle{270.0f, 360.0f, 170.0f, 56.0f}, "Avatar >", true},
            startButton{Rectangle{80.0f, 430.0f, 220.0f, 56.0f}, "Start Game", true},
            backButton{Rectangle{320.0f, 430.0f, 120.0f, 56.0f}, "Back", true},
      avatarPreview{},
      avatarPreviewLoaded(false) {
        setup.playerName = "Hero";
    setup.playerClass = PlayerClass::Knight;
    CollectAvatarFiles();
    LoadPreviewTexture();
}

CharacterCreationScene::~CharacterCreationScene() {
    UnloadPreviewTexture();
}

void CharacterCreationScene::Enter() {
    setup.playerClass = PlayerClass::Knight;
    setup.playerName = "Hero";
    CollectAvatarFiles();
    avatarIndex = 0;
    setup.avatarPath = avatarPaths.empty() ? "" : avatarPaths.front();
    LoadPreviewTexture();
}

CharacterCreationAction CharacterCreationScene::Update() {
    const float sx = static_cast<float>(GetScreenWidth()) / 960.0f;
    const float sy = static_cast<float>(GetScreenHeight()) / 540.0f;

    classButton.bounds = Rectangle{80.0f * sx, 120.0f * sy, 360.0f * sx, 56.0f * sy};
    prevAvatarButton.bounds = Rectangle{80.0f * sx, 360.0f * sy, 170.0f * sx, 56.0f * sy};
    nextAvatarButton.bounds = Rectangle{270.0f * sx, 360.0f * sy, 170.0f * sx, 56.0f * sy};
    startButton.bounds = Rectangle{80.0f * sx, 430.0f * sy, 220.0f * sx, 56.0f * sy};
    backButton.bounds = Rectangle{320.0f * sx, 430.0f * sy, 120.0f * sx, 56.0f * sy};

    if (ui::IsPressed(classButton)) {
        CycleClass();
    }

    if (ui::IsPressed(prevAvatarButton)) {
        PreviousAvatar();
    }

    if (ui::IsPressed(nextAvatarButton)) {
        NextAvatar();
    }

    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 125 && setup.playerName.size() < 16) {
            setup.playerName.push_back(static_cast<char>(key));
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !setup.playerName.empty()) {
        setup.playerName.pop_back();
    }

    if (ui::IsPressed(backButton)) {
        return CharacterCreationAction::Back;
    }

    if (ui::IsPressed(startButton)) {
        if (setup.playerName.empty()) {
            setup.playerName = "Hero";
        }
        return CharacterCreationAction::Start;
    }

    return CharacterCreationAction::None;
}

void CharacterCreationScene::Draw() const {
    const float sx = static_cast<float>(GetScreenWidth()) / 960.0f;
    const float sy = static_cast<float>(GetScreenHeight()) / 540.0f;
    const float sf = std::min(sx, sy);

    DrawText("New Game", static_cast<int>(80.0f * sx), static_cast<int>(36.0f * sy), static_cast<int>(44.0f * sf), RAYWHITE);
    DrawText("1) Choose class  2) Choose avatar  3) Enter name", static_cast<int>(80.0f * sx), static_cast<int>(82.0f * sy), static_cast<int>(24.0f * sf), LIGHTGRAY);

    ui::Button classDisplay = classButton;
    classDisplay.text = std::string("Class: ") + Player::ClassToString(setup.playerClass);
    ui::DrawButton(classDisplay);

    DrawText("Name", static_cast<int>(80.0f * sx), static_cast<int>(196.0f * sy), static_cast<int>(30.0f * sf), RAYWHITE);
    DrawRectangle(static_cast<int>(80.0f * sx), static_cast<int>(230.0f * sy), static_cast<int>(360.0f * sx), static_cast<int>(64.0f * sy), Color{28, 32, 44, 255});
    DrawRectangleLines(static_cast<int>(80.0f * sx), static_cast<int>(230.0f * sy), static_cast<int>(360.0f * sx), static_cast<int>(64.0f * sy), Color{180, 190, 210, 255});
    DrawText(setup.playerName.c_str(), static_cast<int>(94.0f * sx), static_cast<int>(250.0f * sy), static_cast<int>(30.0f * sf), RAYWHITE);

    DrawText("Avatar", static_cast<int>(520.0f * sx), static_cast<int>(120.0f * sy), static_cast<int>(30.0f * sf), RAYWHITE);
    DrawRectangle(static_cast<int>(520.0f * sx), static_cast<int>(160.0f * sy), static_cast<int>(360.0f * sx), static_cast<int>(260.0f * sy), Color{28, 32, 44, 255});
    DrawRectangleLines(static_cast<int>(520.0f * sx), static_cast<int>(160.0f * sy), static_cast<int>(360.0f * sx), static_cast<int>(260.0f * sy), Color{180, 190, 210, 255});

    if (avatarPreviewLoaded) {
        const float scaleX = (320.0f * sx) / static_cast<float>(avatarPreview.width);
        const float scaleY = (220.0f * sy) / static_cast<float>(avatarPreview.height);
        const float scale = std::min(scaleX, scaleY);
        const float drawW = avatarPreview.width * scale;
        const float drawH = avatarPreview.height * scale;
        const float drawX = 520.0f * sx + (360.0f * sx - drawW) * 0.5f;
        const float drawY = 160.0f * sy + (260.0f * sy - drawH) * 0.5f;

        DrawTextureEx(avatarPreview, Vector2{drawX, drawY}, 0.0f, scale, WHITE);
    } else {
        DrawText("No avatar found", static_cast<int>(572.0f * sx), static_cast<int>(280.0f * sy), static_cast<int>(26.0f * sf), GRAY);
    }

    if (!setup.avatarPath.empty()) {
        const std::string fileName = std::filesystem::path(setup.avatarPath).filename().string();
        DrawText(fileName.c_str(), static_cast<int>(520.0f * sx), static_cast<int>(430.0f * sy), static_cast<int>(20.0f * sf), LIGHTGRAY);
    }

    ui::DrawButton(prevAvatarButton);
    ui::DrawButton(nextAvatarButton);
    ui::DrawButton(startButton);
    ui::DrawButton(backButton);
}

CharacterSetupData CharacterCreationScene::GetSetupData() const {
    return setup;
}

void CharacterCreationScene::CollectAvatarFiles() {
    avatarPaths.clear();

    const std::filesystem::path root = ResolveAvatarDirectory();
    if (!std::filesystem::exists(root)) {
        setup.avatarPath.clear();
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(root)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        if (HasImageExtension(entry.path())) {
            avatarPaths.push_back(std::filesystem::absolute(entry.path()).generic_string());
        }
    }

    std::sort(avatarPaths.begin(), avatarPaths.end());

    if (avatarPaths.empty()) {
        setup.avatarPath.clear();
    } else {
        avatarIndex = std::clamp(avatarIndex, 0, static_cast<int>(avatarPaths.size()) - 1);
        setup.avatarPath = avatarPaths[static_cast<size_t>(avatarIndex)];
    }
}

void CharacterCreationScene::LoadPreviewTexture() {
    UnloadPreviewTexture();

    if (setup.avatarPath.empty()) {
        return;
    }

    if (FileExists(setup.avatarPath.c_str())) {
        avatarPreview = LoadTexture(setup.avatarPath.c_str());
        avatarPreviewLoaded = avatarPreview.id != 0;
    }
}

void CharacterCreationScene::UnloadPreviewTexture() {
    if (avatarPreviewLoaded) {
        UnloadTexture(avatarPreview);
        avatarPreviewLoaded = false;
    }
}

void CharacterCreationScene::CycleClass() {
    setup.playerClass = NextClass(setup.playerClass);
}

void CharacterCreationScene::NextAvatar() {
    if (avatarPaths.empty()) {
        setup.avatarPath.clear();
        return;
    }

    avatarIndex = (avatarIndex + 1) % static_cast<int>(avatarPaths.size());
    setup.avatarPath = avatarPaths[static_cast<size_t>(avatarIndex)];
    LoadPreviewTexture();
}

void CharacterCreationScene::PreviousAvatar() {
    if (avatarPaths.empty()) {
        setup.avatarPath.clear();
        return;
    }

    avatarIndex = (avatarIndex - 1 + static_cast<int>(avatarPaths.size())) % static_cast<int>(avatarPaths.size());
    setup.avatarPath = avatarPaths[static_cast<size_t>(avatarIndex)];
    LoadPreviewTexture();
}
