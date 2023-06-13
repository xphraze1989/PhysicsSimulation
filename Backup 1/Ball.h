//ball.h

#pragma once
#include <SFML/Graphics.hpp>

struct Ball {
    sf::CircleShape shape;
    sf::Vector2f velocity;
    bool touchingFloor = false;
    bool touchingWall = false;
    bool touchingBall = false;
    bool processed;
};