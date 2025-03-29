#include "paddle.h"

Paddle::Paddle() : GameObject() {}

Paddle::Paddle(glm::vec2 position, glm::vec2 size, glm::vec3 color, Texture2D sprite)
    : GameObject(position, size, sprite, 0.0f, color) {}

void Paddle::move(float dt, int windowHeight)
{
    Position.y += Velocity.y * dt;
    // Top edge
    if (Position.y <= 0.0f)
    {
        Position.y = 0.0f;
    }
    // Bottom edge
    else if (Position.y + Size.y >= windowHeight)
    {
        Position.y = windowHeight - Size.y;
    }
}