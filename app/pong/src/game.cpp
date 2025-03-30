#include "game.h"

#include "hal/joystick.h"
#include "resourceManager.h"
#include "paddle.h"
#include "util.h"
#include "sounds.h"
#include "rotary_statemachine.h"

Game::Game(int width, int height) : m_width(width), m_height(height)
{
    // Init sound
    sound_init();

    // Init control
    Joystick_init();

    RotaryStateMachine_init();

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
    glm::vec2 ballVelocity(-250.0f, -160.0f);
    float ballRadius = 10.5f;
    m_ball = Ball(ballPos, ballRadius, ballVelocity, ResourceManager::getTexture("ball"));

    glm::vec2 paddleSize(7.5f, 50.0f); 
    glm::vec2 paddlePos(10.0f, m_height / 2 - paddleSize.y / 2);
    glm::vec3 paddleColor(1.0f, 1.0f, 1.0f);
    Paddle paddle(paddlePos, paddleSize, paddleColor, ResourceManager::getTexture("paddle"));
    m_player1 = Player(paddle);

    glm::vec2 paddle2Pos(m_width - 10.0f - paddleSize.x, m_height / 2 - paddleSize.y / 2);
    Paddle paddle2(paddle2Pos, paddleSize, paddleColor, ResourceManager::getTexture("paddle"));
    m_player2 = Player(paddle2);
}

Game::~Game()
{
    // Release texture and shaders 
    ResourceManager::clear();

    // Deinit control
    RotaryStateMachine_cleanup();

    Joystick_deinit();

    // Cleans up sound
    sound_cleanup();
}

void Game::processInput(float dt)
{
    (void) dt;
    JoystickReading reading = Joystick_read();
    // Down
    if (reading.y < JOYSTICK_LOW)
    {
        debugLog("down");
        m_player1.MyPaddle.Velocity.y = 250.0f;
    }
    // Up
    else if (reading.y > JOYSTICK_HIGH)
    {
        debugLog("up");
        m_player1.MyPaddle.Velocity.y = -250.0f;
    }
    // Stay
    else
    {
        m_player1.MyPaddle.Velocity.y = 0.0f;
    }
    // Pressed
    if (Joystick_isPressed())
    {
        debugLog("Pressed");
    }
}

void Game::update(float dt)
{
    m_ball.move(dt, m_width, m_player1, m_player2);
    m_player1.MyPaddle.move(dt, m_height);
    doCollision(m_ball, m_player1.MyPaddle);
    doCollision(m_ball, m_player2.MyPaddle);
}

void Game::render()
{
    // Draw background
    m_spriteRenderer->drawSprite(ResourceManager::getTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(m_width, m_height), 0.0f);

    // Draw gameObject
    m_ball.draw(*m_spriteRenderer);
    m_player1.MyPaddle.draw(*m_spriteRenderer);
    m_player2.MyPaddle.draw(*m_spriteRenderer);

    // Draw UI
    m_uiRenderer->render(m_player1.Score, m_player2.Score);
}

void Game::doCollision(Ball& ball, Paddle paddle)
{
    float halfWidth = paddle.Size.x / 2;
    float halfHeight = paddle.Size.y / 2;

    // Find centers
    glm::vec2 paddleCenter(paddle.Position.x + halfWidth, paddle.Position.y + halfHeight);
    glm::vec2 ballCenter(ball.Position.x + ball.Radius, ball.Position.y + ball.Radius);

    // Find distance vector of ball centre to paddle centre, this vector could be -ve
    glm::vec2 distance(ballCenter.x - paddleCenter.x, ballCenter.y - paddleCenter.y);

    float k = 0.5;
    // Case 1: Ball collide paddle left/ right edge
    if ((halfWidth + ball.Radius) > glm::abs(distance.x) && (halfHeight > glm::abs(distance.y)))
    {
        ball.Velocity.x *= -1; 

        sound_play_collision();

        // Push ball out of the paddle
        if (distance.x > 0) 
            ball.Position.x = paddle.Position.x + paddle.Size.x;
        else 
            ball.Position.x = paddle.Position.x - ball.Size.x;
    
        return;
    }

    // Case 2: Ball collide paddle top/ bottom edge
    if ((halfHeight + ball.Radius) > glm::abs(distance.y) && (halfWidth > glm::abs(distance.x)))
    {
        ball.Velocity.y = -ball.Velocity.y + k * paddle.Velocity.y; 

        sound_play_collision();

        // Push ball out of the paddle
        if (distance.y > 0) 
            ball.Position.y = paddle.Position.y + paddle.Size.y;
        else 
            ball.Position.y = paddle.Position.y - ball.Size.y;

        return;
    }

    // Case 3: Ball collide paddle corner
    float cornerDistanceX = glm::abs(distance.x) - halfWidth;
    float cornerDistanceY = glm::abs(distance.y) - halfHeight;
    float cornerDistSq = glm::pow(cornerDistanceX, 2) + glm::pow(cornerDistanceY, 2);
    if (cornerDistSq < glm::pow(ball.Radius, 2))
    {
        ball.Velocity.x *= -1;
        ball.Velocity.y = -ball.Velocity.y + k * paddle.Velocity.y; 

        sound_play_collision();
    }

}
