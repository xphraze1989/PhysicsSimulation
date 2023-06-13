//main.cpp

#include "Global.h"
#include "Gridsystem.h"

// Define the variables here
std::vector<Ball> balls;
std::default_random_engine generator;
std::uniform_real_distribution<float> distribution(-100.0, 100.0);
sf::Font font;
sf::Text ballCountText;
sf::Clock fpsClock;
sf::Text fpsText;

bool continuousSpawning = false;

void SpawnBallAtMousePosition(sf::RenderWindow& window);
void InitializeTexts();
void UpdateAndDrawTexts(sf::RenderWindow& window);
void SpawnBallAtMousePosition(sf::RenderWindow& window);


int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Physics Simulation", sf::Style::Close | sf::Style::Titlebar);
    window.setFramerateLimit(Framelimiter); // Limit the framerate to 60 frames per second

    // Initialize the grid system
    GridSystem gridSystem(window.getSize().x, window.getSize().y);

    // Initialize the balls vector
    std::vector<Grid::Ball*> balls;

    // Initialize the font
    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        // handle error
    }

    bool continuousSpawning = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonPressed && !continuousSpawning) {
                Grid::Ball* newBall = new Grid::Ball();
                newBall->shape.setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
                balls.push_back(newBall);
                gridSystem.addBallToGrid(newBall);
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    continuousSpawning = !continuousSpawning;
                }
                else if (event.key.code == sf::Keyboard::X) {
                    for (Ball* ball : balls) {
                        gridSystem.removeBallFromGrid(ball);
                        delete ball;
                    }
                    balls.clear();
                }
            }
        }

        if (continuousSpawning) {
            Ball* newBall = new Ball();
            newBall->shape.setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
            balls.push_back(newBall);
            gridSystem.addBallToGrid(newBall);
        }

        // Update the grid system
        gridSystem.update(balls);

        window.clear();

        // Draw the balls
        for (auto& ball : balls) {
            window.draw(ball->shape);
        }

        // Draw the grid system
        gridSystem.draw(window, font);

        window.display();
    }

    return 0;
}


void SpawnBallAtMousePosition(sf::RenderWindow& window) {
    Ball ball;
    ball.shape = sf::CircleShape(ballsize); // Use ballsize here
    ball.shape.setFillColor(sf::Color::Green);
    ball.shape.setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);

    ball.shape = sf::CircleShape(ballsize); // Use ballsize here
    ball.shape.setFillColor(sf::Color::Green);
    ball.shape.setPosition(sf::Mouse::getPosition(window).x-3, sf::Mouse::getPosition(window).y);

    ball.shape = sf::CircleShape(ballsize); // Use ballsize here
    ball.shape.setFillColor(sf::Color::Green);
    ball.shape.setPosition(sf::Mouse::getPosition(window).x+3, sf::Mouse::getPosition(window).y);

    // Generate a random velocity for the ball
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(-100.0, 100.0);
    ball.velocity = sf::Vector2f(distribution(generator), distribution(generator));

    balls.push_back(ball);
}

void InitializeTexts() {
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cout << "Error loading font\n";
    }

    ballCountText.setFont(font);
    ballCountText.setCharacterSize(24); // in pixels
    ballCountText.setFillColor(sf::Color::White);
    ballCountText.setPosition(10, 10); // top-left corner of the window

    fpsText.setFont(font);
    fpsText.setCharacterSize(24); // in pixels
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(200, 10); // top-left corner of the window, to the right of the ball count
}

void UpdateAndDrawTexts(sf::RenderWindow& window) {
    // Update the ball count text
    std::stringstream ss;
    ss << "Ball count: " << balls.size();
    ballCountText.setString(ss.str());

    // Update the FPS text
    std::stringstream fps_ss;
    float fps = 1.0f / fpsClock.restart().asSeconds();
    fps_ss << "FPS: " << fps;
    fpsText.setString(fps_ss.str());

    // Draw the texts
    window.draw(ballCountText);
    window.draw(fpsText);
}

void SpawnBallAtMousePosition(sf::RenderWindow& window) {
    Ball* newBall = new Ball();
    newBall->shape.setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
    balls.push_back(newBall);
    gridSystem.addBallToGrid(newBall);
}