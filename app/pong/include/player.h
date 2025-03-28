#pragma once

#include "paddle.h"

class Player
{
public:
    int    Score;
    Paddle MyPaddle;

    Player();
    Player(Paddle paddle);
};