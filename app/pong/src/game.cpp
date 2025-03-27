#include "game.h"
#include "resourceManager.h"
#include "spriteRenderer.h"
#include "hal/joystick.h"

Game::Game(int width, int height) : m_width(width), m_height(height)
{ 
}

Game::~Game()
{
}

void Game::init()
{
    // Load shaders
    ResourceManager::loadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");

    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, -1.0f, 1.0f);
    ResourceManager::getShader("sprite").use().setInteger("image", 0);
    ResourceManager::getShader("sprite").setMatrix4("projection", projection);

    // Load textures
    ResourceManager::loadTexture("textures/background.jpg", false, "background");
    // Init renderer
    m_renderer = std::make_unique<SpriteRenderer>(ResourceManager::getShader("sprite"));
}

void Game::update(float dt)
{
    (void) dt;
}

void Game::processInput(float dt)
{
    (void) dt;
}

void Game::render()
{
    m_renderer->drawSprite(ResourceManager::getTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(m_width, m_height), 0.0f);
}