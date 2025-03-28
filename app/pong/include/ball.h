#pragma once

#include "gameObject.h"
#include "texture.h"

#include <glm/glm.hpp>

// Ball holds the state of the Ball object inheriting
// relevant state data from GameObject. Contains some extra
// functionality specific to Breakout's ball object that
// were too specific for within GameObject alone.
class Ball : public GameObject
{
public:
    // Constructors
    Ball();
    Ball(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);
    
    // Moves the ball, keeping it constrained within the window bounds (except left and right edge); returns new position
    glm::vec2 move(float dt, int windowWidth);
    
    // Reset the ball to original state with given position and velocity
    void      reset(glm::vec2 position, glm::vec2 velocity);

private:
    // Ball state
    float m_radius;
};