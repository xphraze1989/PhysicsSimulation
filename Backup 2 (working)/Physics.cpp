//Physics.cpp

#include "Global.h"
#include "physics.h"


void CheckSelfCollisionThread(int start, int end) {
    for (int i = start; i < end; ++i) {
        for (int j = i + 1; j < balls.size(); ++j) {
            Ball& ball = balls[i];
            Ball& other = balls[j];

            sf::Vector2f delta = other.shape.getPosition() - ball.shape.getPosition();
            float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
            float combinedRadius = ball.shape.getRadius() + other.shape.getRadius();

            if (distance < combinedRadius) {
                // The balls are colliding
                ball.touchingBall = true;
                other.touchingBall = true;
                Unintersect(ball, other);
            }
        }
    }
}

void CheckSelfCollision() {
    int numThreads = std::thread::hardware_concurrency(); // Get the number of cores
    std::vector<std::thread> threads(numThreads);
    int ballsPerThread = balls.size() / numThreads;

    for (int i = 0; i < numThreads; ++i) {
        int start = i * ballsPerThread;
        int end = (i == numThreads - 1) ? balls.size() : start + ballsPerThread;
        threads[i] = std::thread(CheckSelfCollisionThread, start, end);
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

