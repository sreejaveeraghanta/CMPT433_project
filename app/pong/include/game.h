#pragma once

#include "ball.h"
#include "spriteRenderer.h"
#include "uiRenderer.h"
#include "player.h"
#include <memory>

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // Game must only be constructed after EGL is set up
    // Init textures, shaders, gameObjects etc.
    Game(int width, int height);
    
    ~Game();

    // Process  external input, should be called every frame
    void processInput(float dt);

    // Update game state, should be called every frame
    void update(float dt);

    // Render new frame to GPU framebuffer, should be called every frame
    void render();
    
private:
    // Window dimensions
    int m_width;
    int m_height;
    
    // Renderers
    std::unique_ptr<SpriteRenderer> m_spriteRenderer;
    std::unique_ptr<UIRenderer>     m_uiRenderer;

    // Game objects
    Ball m_ball;

    // Players
    Player m_player1; // Left
    Player m_player2; // Right

    void doCollision(Ball& ball, Paddle paddle);
};
