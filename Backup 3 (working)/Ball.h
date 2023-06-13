//ball.h

#pragma once
#include <SFML/Graphics.hpp>

struct Ball {
    sf::CircleShape shape;
    sf::Vector2f velocity;
    bool touchingFloor = false;
    bool touchingWall = false;
    bool touchingBall = false;
    bool processed = false; // Initialize processed

    Ball() {
        shape.setRadius(10.0f);
        shape.setFillColor(sf::Color::Red);
        velocity = sf::Vector2f(0.0f, 0.0f);
    }
};