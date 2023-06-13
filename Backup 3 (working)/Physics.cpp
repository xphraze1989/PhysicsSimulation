//Physics.cpp

#include "Ball.h"
#include "Global.h"
#include "physics.h"
#include "Gridsystem.h"


void CheckGridCollisionThread(int start, int end, GridSystem& gridSystem) {
    const auto& grids = gridSystem.getGrids();
    for (int i = start; i < end; ++i) {
        for (int j = 0; j < grids[i].size(); ++j) {
            for (Ball* ball : grids[i][j].balls) {
                // Check for collisions with balls in the same grid
                for (Ball* other : grids[i][j].balls) {
                    if (ball != other) {
                        sf::Vector2f delta = other->shape.getPosition() - ball->shape.getPosition();
                        float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
                        float combinedRadius = ball->shape.getRadius() + other->shape.getRadius();

                        if (distance < combinedRadius) {
                            // The balls are colliding
                            ball->touchingBall = true;
                            other->touchingBall = true;
                            Unintersect(*ball, *other);
                        }
                    }
                }

                // Check for collisions with balls in neighboring grids
                for (int dx = -1; dx <= 1; ++dx) {
                    for (int dy = -1; dy <= 1; ++dy) {
                        if (dx == 0 && dy == 0) continue;  // Skip the current grid, as we already checked it

                        int neighborX = i + dx;
                        int neighborY = j + dy;  // Change from i + dy to j + dy

                        // Skip if the neighboring grid is out of bounds or being processed by another thread
                        if (neighborX < 0 || neighborX >= grids.size() || neighborY < 0 || neighborY >= grids[0].size() || neighborX < start || neighborX >= end) continue;

                        for (Ball* other : grids[neighborX][neighborY].balls) {
                            sf::Vector2f delta = other->shape.getPosition() - ball->shape.getPosition();
                            float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
                            float combinedRadius = ball->shape.getRadius() + other->shape.getRadius();

                            if (distance < combinedRadius) {
                                // The balls are colliding
                                ball->touchingBall = true;
                                other->touchingBall = true;
                                Unintersect(*ball, *other);
                            }
                        }
                    }
                }
            }
        }
    }
}


void CheckGridCollision(GridSystem& gridSystem) { // Add GridSystem& gridSystem as an argument
    size_t numThreads = std::thread::hardware_concurrency(); // Get the number of cores
    std::vector<std::thread> threads(numThreads);
    size_t gridsPerThread = gridSystem.getGrids().size() / numThreads;

    for (size_t i = 0; i < numThreads; ++i) {
        size_t start = i * gridsPerThread;
        size_t end = (i == numThreads - 1) ? gridSystem.getGrids().size() : start + gridsPerThread;
        threads[i] = std::thread(CheckGridCollisionThread, start, end, std::ref(gridSystem));
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

void Unintersect(Ball& ball, Ball& other) {
    sf::Vector2f delta = other.shape.getPosition() - ball.shape.getPosition();
    float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    sf::Vector2f deltaNormalized = delta / distance;

    // Calculate the velocity along the direction from ball to other
    sf::Vector2f relativeVelocity = other.velocity - ball.velocity;
    float velocityAlongNormal = relativeVelocity.x * deltaNormalized.x + relativeVelocity.y * deltaNormalized.y;

    // If the velocities are separating, no need to resolve the collision
    if (velocityAlongNormal > 0)
        return;

    // Calculate the impulse scalar
    float impulseScalar = -(1 + 1) * velocityAlongNormal;
    impulseScalar /= ball.shape.getRadius() + other.shape.getRadius();

    // Apply the impulse
    sf::Vector2f impulse = impulseScalar * deltaNormalized;
    ball.velocity -= ball.shape.getRadius() * impulse;
    other.velocity += other.shape.getRadius() * impulse;

    // Move the balls apart
    float overlap = ball.shape.getRadius() + other.shape.getRadius() - distance;
    sf::Vector2f separation = deltaNormalized * overlap;
    ball.shape.move(-separation / 2.0f);
    other.shape.move(separation / 2.0f);
}

void CollisionExecute(Ball& ball) {
    for (auto& other : balls) {
        if (&ball != &other && ball.touchingBall && other.touchingBall) {
            sf::Vector2f delta = other.shape.getPosition() - ball.shape.getPosition();
            float collisionAngle = std::atan2(delta.y, delta.x);

            // Calculate the new velocities of the balls after the collision
            float magnitude1 = std::sqrt(ball.velocity.x * ball.velocity.x + ball.velocity.y * ball.velocity.y);
            float magnitude2 = std::sqrt(other.velocity.x * other.velocity.x + other.velocity.y * other.velocity.y);
            float direction1 = std::atan2(ball.velocity.y, ball.velocity.x);
            float direction2 = std::atan2(other.velocity.y, other.velocity.x);
            float new_x_velocity1 = magnitude1 * std::cos(direction1 - collisionAngle);
            float new_y_velocity1 = magnitude2 * std::sin(direction2 - collisionAngle);
            float new_x_velocity2 = magnitude2 * std::cos(direction2 - collisionAngle);
            float new_y_velocity2 = magnitude1 * std::sin(direction1 - collisionAngle);

            // Update the velocities of the balls
            ball.velocity.x = std::cos(collisionAngle) * new_x_velocity1 + std::cos(collisionAngle + M_PI / 2) * new_y_velocity1;
            ball.velocity.y = std::sin(collisionAngle) * new_x_velocity1 + std::sin(collisionAngle + M_PI / 2) * new_y_velocity1;
            other.velocity.x = std::cos(collisionAngle) * new_x_velocity2 + std::cos(collisionAngle + M_PI / 2) * new_y_velocity2;
            other.velocity.y = std::sin(collisionAngle) * new_x_velocity2 + std::sin(collisionAngle + M_PI / 2) * new_y_velocity2;
        }
    }
}

void Integrate(sf::Time dt) {

    for (auto& ball : balls) {
        // Apply gravity by increasing the y-velocity
        ball.velocity.y += gravity * dt.asSeconds();

        // Update the position based on the velocity
        ball.shape.move(ball.velocity * dt.asSeconds());
    }
}

void HandleWallAndGroundCollision(Ball& ball) {

    // If the ball hits the left edge of the window, reverse its x-velocity and adjust its position
    if (ball.shape.getPosition().x - ball.shape.getRadius() < 0) {
        ball.velocity.x = -ball.velocity.x * wallBounceFactor;
        ball.shape.setPosition(ball.shape.getRadius(), ball.shape.getPosition().y);
    }

    // If the ball hits the right edge of the window, reverse its x-velocity and adjust its position
    if (ball.shape.getPosition().x + ball.shape.getRadius() > windowWidth) {
        ball.velocity.x = -ball.velocity.x * wallBounceFactor;
        ball.shape.setPosition(windowWidth - ball.shape.getRadius(), ball.shape.getPosition().y);
    }

    // If the ball hits the top edge of the window, reverse its y-velocity and adjust its position
    if (ball.shape.getPosition().y - ball.shape.getRadius() < 0) {
        ball.velocity.y = -ball.velocity.y * wallBounceFactor;
        ball.shape.setPosition(ball.shape.getPosition().x, ball.shape.getRadius());
    }

    // If the ball hits the bottom edge of the window, reverse its y-velocity and adjust its position
    if (ball.shape.getPosition().y + ball.shape.getRadius() > windowHeight) {
        ball.velocity.y = -ball.velocity.y * wallBounceFactor;
        ball.shape.setPosition(ball.shape.getPosition().x, windowHeight - ball.shape.getRadius());
    }
}

