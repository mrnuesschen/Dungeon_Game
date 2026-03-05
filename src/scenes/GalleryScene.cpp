#include "scenes/GalleryScene.h"

#include <algorithm>

#include "raylib.h"

GalleryScene::GalleryScene()
    : backButton{Rectangle{40.0f, 36.0f, 190.0f, 54.0f}, "Back", true} {}

bool GalleryScene::Update() {
    const float sx = static_cast<float>(GetScreenWidth()) / 960.0f;
    const float sy = static_cast<float>(GetScreenHeight()) / 540.0f;
    backButton.bounds = Rectangle{40.0f * sx, 36.0f * sy, 190.0f * sx, 54.0f * sy};
    return ui::IsPressed(backButton);
}

void GalleryScene::Draw(const GalleryData& gallery) const {
    const float sx = static_cast<float>(GetScreenWidth()) / 960.0f;
    const float sy = static_cast<float>(GetScreenHeight()) / 540.0f;
    const float sf = std::min(sx, sy);

    DrawText("Gallery", static_cast<int>(405.0f * sx), static_cast<int>(36.0f * sy), static_cast<int>(44.0f * sf), RAYWHITE);
    ui::DrawButton(backButton);

    const int cardWidth = static_cast<int>(360.0f * sx);
    const int cardHeight = static_cast<int>(170.0f * sy);
    const int left = static_cast<int>(90.0f * sx);
    const int top = static_cast<int>(120.0f * sy);
    const int spacingX = static_cast<int>(420.0f * sx);
    const int spacingY = static_cast<int>(190.0f * sy);

    for (int i = 0; i < 4; ++i) {
        const int col = i % 2;
        const int row = i / 2;
        const int x = left + col * spacingX;
        const int y = top + row * spacingY;

        DrawRectangle(x, y, cardWidth, cardHeight, Color{36, 42, 60, 255});
        DrawRectangleLines(x, y, cardWidth, cardHeight, GRAY);

        if (gallery.unlocked[static_cast<size_t>(i)]) {
            DrawRectangle(x + 12, y + 12, cardWidth - 24, cardHeight - 24, Color{95, 72, 160, 255});
            DrawCircle(x + 75, y + 85, 34.0f, Color{236, 206, 75, 255});
            DrawText(TextFormat("Image %d", i + 1), x + static_cast<int>(230.0f * sx), y + static_cast<int>(75.0f * sy), static_cast<int>(28.0f * sf), RAYWHITE);
            DrawText("Unlocked", x + static_cast<int>(240.0f * sx), y + static_cast<int>(110.0f * sy), static_cast<int>(22.0f * sf), LIGHTGRAY);
        } else {
            DrawRectangle(x + 12, y + 12, cardWidth - 24, cardHeight - 24, Color{54, 54, 64, 255});
            DrawText("LOCKED", x + static_cast<int>(130.0f * sx), y + static_cast<int>(72.0f * sy), static_cast<int>(40.0f * sf), DARKGRAY);
        }
    }
}
