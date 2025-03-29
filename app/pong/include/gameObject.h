#pragma once

#include "texture.h"
#include "spriteRenderer.h"

#include <glm/glm.hpp>

// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.
class GameObject
{
public:
    // Object state
    glm::vec2   Position, Size, Velocity;
    glm::vec3   Color;
    float       Rotation;

    // Constructors
    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, float rotation, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
        
    // Draw sprite
    virtual void draw(SpriteRenderer &renderer);

protected:
    bool        m_destroyed;

    // Render state
    Texture2D   m_sprite;	
};
