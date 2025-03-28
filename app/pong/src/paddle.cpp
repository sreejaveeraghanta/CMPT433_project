#include "paddle.h"

Paddle::Paddle() : GameObject() {}

Paddle::Paddle(glm::vec2 position, glm::vec2 size, glm::vec3 color, Texture2D sprite)
    : GameObject(position, size, sprite, 0.0f, color) {}

void Paddle::move(float pixels, int windowHeight)
{
    m_position.y += pixels;
    // Top edge
    if (m_position.y <= 0.0f)
    {
        m_position.y = 0.0f;
    }
    // Bottom edge
    else if (m_position.y + m_size.y >= windowHeight)
    {
        m_position.y = windowHeight - m_size.y;
    }
}