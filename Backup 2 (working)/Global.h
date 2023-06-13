//Global.h

#ifndef GLOBAL_H
#define GLOBAL_H

#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <sstream>
#include <iostream>
#include <thread>

#include "Ball.h" // Include this
#include "physics.h"

const float wallBounceFactor = 0.8f; // Adjust as needed

const float windowWidth = 800.0f;
const float windowHeight = 600.0f;
const int Framelimiter = 60;
const float gravity = 200.0f; // Adjust as needed
const float ballsize = 4.0f;

void SpawnBallAtMousePosition(sf::RenderWindow& window);



extern std::vector<Ball> balls;
extern std::default_random_engine generator;
extern std::uniform_real_distribution<float> distribution;
extern sf::Font font;
extern sf::Text ballCountText;
extern sf::Clock fpsClock;
extern sf::Text fpsText;

#endif // GLOBAL_H