#pragma once

#include "PVRUtils/PVRUtilsGles.h"

class UIRenderer
{
public:
    UIRenderer();
    UIRenderer(int width, int height);
    void render(int player1Score, int player2SCore);

private:
    pvr::ui::UIRenderer m_renderer;
    pvr::ui::Text m_score1;
    pvr::ui::Text m_score2;
};