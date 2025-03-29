#include "gameObject.h"

GameObject::GameObject() 
    : Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), m_sprite() { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, float rotation, glm::vec3 color, glm::vec2 velocity) 
    : Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(rotation), m_sprite(sprite) { }

void GameObject::draw(SpriteRenderer &renderer)
{
    renderer.drawSprite(m_sprite, Position, Size, Rotation, Color);
}