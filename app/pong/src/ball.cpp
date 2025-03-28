#include "ball.h"


Ball::Ball() 
    : GameObject(), m_radius(8.5f) { }

Ball::Ball(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite)
    : GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f), sprite, 0.0f, glm::vec3(1.0f), velocity), m_radius(radius) { }

glm::vec2 Ball::move(float dt, int windowWidth)
{
    // Move the ball
    m_position += m_velocity * dt;

    // Left edge
    if (m_position.x <= 0.0f)
    {
        m_velocity.x = -m_velocity.x;
        m_position.x = 0.0f;
    }
    // Right edge
    else if (m_position.x + m_size.x >= windowWidth)
    {
        m_velocity.x = -m_velocity.x;
        m_position.x = windowWidth - m_size.x;
    }
    // Top edge
    if (m_position.y <= 0.0f)
    {
        m_velocity.y = -m_velocity.y;
        m_position.y = 0.0f;
    }
    // Bottom edge
    else if (m_position.y + m_size.y >= windowWidth)
    {
        m_velocity.y = -m_velocity.y;
        m_position.y = windowWidth - m_size.y;
    }
    
    return m_position;
}

// Resets the ball to initial Stuck Position (if ball is outside window bounds)
void Ball::reset(glm::vec2 position, glm::vec2 velocity)
{
    m_position = position;
    m_velocity = velocity;
}