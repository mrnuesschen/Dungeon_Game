#pragma once

#include <string>

#include "raylib.h"

namespace ui {

struct Button {
    Rectangle bounds;
    std::string text;
    bool enabled = true;
};

bool IsPressed(const Button& button);
void DrawButton(const Button& button);

}
