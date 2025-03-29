#pragma once

#include "gameObject.h"

class Paddle : public GameObject
{
public:
    Paddle();
    Paddle(glm::vec2 position, glm::vec2 size, glm::vec3 color, Texture2D sprite);

    void move(float dt, int windowHeight);
};