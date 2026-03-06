#include "ui/Button.h"

#include <algorithm>

namespace ui {

namespace {

Rectangle GetVisualButtonRect(const Button& button) {
    constexpr float kButtonScale = 0.75f;
    const float scaledWidth = button.bounds.width * kButtonScale;
    const float scaledHeight = button.bounds.height * kButtonScale;
    const float offsetX = (button.bounds.width - scaledWidth) * 0.5f;
    const float offsetY = (button.bounds.height - scaledHeight) * 0.5f;

    return Rectangle{
        button.bounds.x + offsetX,
        button.bounds.y + offsetY,
        scaledWidth,
        scaledHeight};
}

} // namespace

bool IsPressed(const Button& button) {
    const Rectangle visualRect = GetVisualButtonRect(button);
    return button.enabled && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), visualRect);
}

void DrawButton(const Button& button) {
    const Rectangle visualRect = GetVisualButtonRect(button);
    const bool hovered = button.enabled && CheckCollisionPointRec(GetMousePosition(), visualRect);
    Color color = button.enabled ? Color{44, 116, 214, 255} : Color{75, 75, 75, 255};

    if (hovered) {
        color = Color{58, 132, 232, 255};
    }

    DrawRectangleRec(visualRect, color);
    DrawRectangleLinesEx(visualRect, 2.0f, Color{220, 220, 230, 255});

    const int fontSize = std::clamp(static_cast<int>(visualRect.height * 0.45f), 16, 34);
    const int textWidth = MeasureText(button.text.c_str(), fontSize);
    const int textX = static_cast<int>(visualRect.x + (visualRect.width - static_cast<float>(textWidth)) * 0.5f);
    const int textY = static_cast<int>(visualRect.y + (visualRect.height - static_cast<float>(fontSize)) * 0.5f);
    DrawText(button.text.c_str(), textX, textY, fontSize, RAYWHITE);
}

}
