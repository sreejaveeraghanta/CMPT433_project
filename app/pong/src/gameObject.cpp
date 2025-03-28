#include "gameObject.h"

GameObject::GameObject() 
    : m_position(0.0f, 0.0f), m_size(1.0f, 1.0f), m_velocity(0.0f), m_color(1.0f), m_rotation(0.0f), m_destroyed(false), m_sprite() { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, float rotation, glm::vec3 color, glm::vec2 velocity) 
    : m_position(pos), m_size(size), m_velocity(velocity), m_color(color), m_rotation(rotation), m_destroyed(false), m_sprite(sprite) { }

void GameObject::draw(SpriteRenderer &renderer)
{
    renderer.drawSprite(m_sprite, m_position, m_size, m_rotation, m_color);
}