#include "ball.h"

#include <random>
#include <cstdlib>  // For rand()
#include <ctime>    // For time()

Ball::Ball() 
    : GameObject(), Radius(8.5f) { }

Ball::Ball(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite)
    : GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f), sprite, 0.0f, glm::vec3(1.0f), velocity),
        Radius(radius), m_defaultPosition(pos), m_defaultVelocity(velocity) { }

void Ball::move(float dt, int windowWidth, Player& player1, Player& player2)
{
    // Move the ball
    Position += Velocity * dt;

    // Left edge
    if (Position.x <= 0.0f)
    {
        player2.Score += 1;
        reset();
    }
    // Right edge
    else if (Position.x + Size.x >= windowWidth)
    {
        Velocity.x = -Velocity.x;
        Position.x = windowWidth - Size.x;
    }
    // Top edge
    if (Position.y <= 0.0f)
    {
        Velocity.y = -Velocity.y;
        Position.y = 0.0f;
    }
    // Bottom edge
    else if (Position.y + Size.y >= windowWidth)
    {
        Velocity.y = -Velocity.y;
        Position.y = windowWidth - Size.y;
    }
}

// Resets the ball to initial Stuck Position (if ball is outside window bounds)
void Ball::reset()
{
    Position = m_defaultPosition;
    
    // Shoot to random side at random angle +-30deg
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> angleDist(0, 30);
    std::uniform_int_distribution<std::mt19937::result_type> xDist(0, 1);
    std::uniform_int_distribution<std::mt19937::result_type> yDist(0, 1);

    float toX = xDist(rng) == 0 ? 1.0 : -1.0;
    float toY = xDist(rng) == 0 ? 1.0 : -1.0;
    float angleDeg = angleDist(rng);
    float angleRad = glm::radians(angleDeg);

    glm::vec2 unitVelocity(glm::cos(angleRad), glm::sin(angleRad));
    float speed = glm::length(m_defaultVelocity);
    Velocity = unitVelocity * speed;
    Velocity.x *= toX;
    Velocity.y *= toY;
}