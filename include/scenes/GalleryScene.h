#pragma once

#include "GameTypes.h"
#include "ui/Button.h"

class GalleryScene {
public:
    GalleryScene();

    bool Update();
    void Draw(const GalleryData& gallery) const;

private:
    ui::Button backButton;
};
