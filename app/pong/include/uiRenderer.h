#pragma once

#include "PVRUtils/PVRUtilsGles.h"

class UIRenderer
{
public:
    UIRenderer();
    UIRenderer(int width, int height);
    void render();

private:
    pvr::ui::UIRenderer m_renderer;
};