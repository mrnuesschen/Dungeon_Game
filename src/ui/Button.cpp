#include "ui/Button.h"

#include <algorithm>

namespace ui {

bool IsPressed(const Button& button) {
    return button.enabled && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), button.bounds);
}

void DrawButton(const Button& button) {
    const bool hovered = button.enabled && CheckCollisionPointRec(GetMousePosition(), button.bounds);
    Color color = button.enabled ? Color{44, 116, 214, 255} : Color{75, 75, 75, 255};

    if (hovered) {
        color = Color{58, 132, 232, 255};
    }

    DrawRectangleRec(button.bounds, color);
    DrawRectangleLinesEx(button.bounds, 2.0f, Color{220, 220, 230, 255});

    const int fontSize = std::clamp(static_cast<int>(button.bounds.height * 0.45f), 16, 34);
    const int textWidth = MeasureText(button.text.c_str(), fontSize);
    const int textX = static_cast<int>(button.bounds.x + (button.bounds.width - static_cast<float>(textWidth)) * 0.5f);
    const int textY = static_cast<int>(button.bounds.y + (button.bounds.height - static_cast<float>(fontSize)) * 0.5f);
    DrawText(button.text.c_str(), textX, textY, fontSize, RAYWHITE);
}

}
