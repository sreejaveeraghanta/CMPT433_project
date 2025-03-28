#include "game.h"

#include "hal/joystick.h"
#include "resourceManager.h"
#include "paddle.h"
#include "util.h"

Game::Game(int width, int height) : m_width(width), m_height(height)
{
    // Init control
    Joystick_init();

    // Load shaders
    ResourceManager::loadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");

    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, -1.0f, 1.0f);
    ResourceManager::getShader("sprite").use().setInteger("image", 0);
    ResourceManager::getShader("sprite").setMatrix4("projection", projection);

    // Load textures
    ResourceManager::loadTexture("textures/background.jpg", false, "background");
    ResourceManager::loadTexture("textures/ball.png", true, "ball");
    ResourceManager::loadTexture("textures/paddle.png", true, "paddle");

    // Init renderer, after loading resources
    m_spriteRenderer = std::make_unique<SpriteRenderer>(ResourceManager::getShader("sprite"));
    m_uiRenderer = std::make_unique<UIRenderer>(m_width, m_height);

    // Init gameObjects
    glm::vec2 ballPos(m_width / 2.0f, m_height / 2.0f);
    glm::vec2 ballVelocity(-120.0f, -80.0f);
    float ballRadius = 15.0f;
    m_ball = Ball(ballPos, ballRadius, ballVelocity, ResourceManager::getTexture("ball"));

    glm::vec2 paddleSize(10.0f, 70.0f); 
    glm::vec2 paddlePos(paddleSize.x * 2, m_height / 2 - paddleSize.y / 2);
    glm::vec3 paddleColor(1.0f, 1.0f, 1.0f);
    Paddle paddle(paddlePos, paddleSize, paddleColor, ResourceManager::getTexture("paddle"));
    m_player1 = Player(paddle);
}

Game::~Game()
{
    // Release texture and shaders 
    ResourceManager::clear();

    // Deinit control
    Joystick_deinit();
}

void Game::processInput(float dt)
{
    (void) dt;
    JoystickReading reading = Joystick_read();
    // Left
    if (reading.x < JOYSTICK_LOW)
    {
        debugLog("Left");
    }
    // Right
    else if (reading.x > JOYSTICK_HIGH)
    {
        debugLog("Right");
    }
    // Down
    if (reading.y < JOYSTICK_LOW)
    {
        m_player1.MyPaddle.move(10.0f, m_height);
        debugLog("Down");
    }
    // Up
    else if (reading.y > JOYSTICK_HIGH)
    {
        m_player1.MyPaddle.move(-10.0f, m_height);
        debugLog("Up");
    }
    // Pressed
    if (Joystick_isPressed())
    {
        debugLog("Pressed");
    }
}

void Game::update(float dt)
{
    (void) dt;
    m_ball.move(dt, m_width);
}

void Game::render()
{
    // Draw background
    m_spriteRenderer->drawSprite(ResourceManager::getTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(m_width, m_height), 0.0f);

    // Draw gameObject
    m_ball.draw(*m_spriteRenderer);
    m_player1.MyPaddle.draw(*m_spriteRenderer);

    // Draw UI
    m_uiRenderer->render();
}