#pragma once

#include "spriteRenderer.h"

#include <memory>

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    Game(int width, int height);
    ~Game();
    void init();
    void processInput(float dt);
    void update(float dt);
    void render();
    
private:
    int                             m_width;
    int                             m_height;
    std::unique_ptr<SpriteRenderer> m_renderer;
};
