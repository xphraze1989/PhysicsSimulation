#pragma once

#include "Global.h" // Include this before the function declarations
#include "Ball.h"
#include "Gridsystem.h"

void CheckSelfCollision();
void Unintersect(Ball& ball, Ball& other);
void CollisionExecute(Ball& ball);
void Integrate(sf::Time dt);
void HandleWallAndGroundCollision(Ball& ball);
void CheckSelfCollisionThread(int start, int end, GridSystem& gridSystem);