//Gridsystem.h

#pragma once
#include <vector>
#include "Ball.h"
#include <SFML/Graphics.hpp>

class Grid {
public:
    std::vector<Ball*> balls;  // The balls currently in this grid

    void addBall(Ball* ball) {
        balls.push_back(ball);
    }

    void removeBall(Ball* ball) {
        balls.erase(std::remove(balls.begin(), balls.end(), ball), balls.end());
    }

    int getBallCount() const {
        return balls.size();
    }
};

class GridSystem {
public:
    static const int GRID_SIZE = 100;  // The size of each grid

    GridSystem(int screenWidth, int screenHeight) {
        // Calculate the number of grids needed to cover the screen
        int gridCountX = screenWidth / GRID_SIZE;
        int gridCountY = screenHeight / GRID_SIZE;

        // Create the grids
        for (int i = 0; i < gridCountX; i++) {
            std::vector<Grid> gridRow;
            for (int j = 0; j < gridCountY; j++) {
                gridRow.push_back(Grid());
            }
            grids.push_back(gridRow);
        }
    }

    // Function to add a ball to the correct grid based on its position
    void addBallToGrid(Ball* ball) {
        int gridX = ball->shape.getPosition().x / GRID_SIZE;
        int gridY = ball->shape.getPosition().y / GRID_SIZE;

        grids[gridX][gridY].addBall(ball);
    }

    // Function to remove a ball from a grid
    void removeBallFromGrid(Ball* ball) {
        int gridX = ball->shape.getPosition().x / GRID_SIZE;
        int gridY = ball->shape.getPosition().y / GRID_SIZE;

        grids[gridX][gridY].removeBall(ball);
    }

    void update(std::vector<Ball>& balls) {
        // Clear all grids
        for (auto& gridRow : grids) {
            for (auto& grid : gridRow) {
                grid.balls.clear();
            }
        }

        // Add each ball to the correct grid
        for (auto& ball : balls) {
            addBallToGrid(&ball);
        }
    }


    void draw(sf::RenderWindow& window, sf::Font& font) {
        for (int i = 0; i < grids.size(); i++) {
            for (int j = 0; j < grids[0].size(); j++) {
                sf::RectangleShape rectangle(sf::Vector2f(GRID_SIZE, GRID_SIZE));
                rectangle.setPosition(i * GRID_SIZE, j * GRID_SIZE);
                rectangle.setFillColor(sf::Color::Transparent);
                rectangle.setOutlineColor(sf::Color::White);
                rectangle.setOutlineThickness(1);

                window.draw(rectangle);

                sf::Text text;
                text.setFont(font);
                text.setString(std::to_string(grids[i][j].getBallCount()));
                text.setCharacterSize(12);
                text.setFillColor(sf::Color::White);
                text.setPosition(i * GRID_SIZE, j * GRID_SIZE);

                window.draw(text);
            }
        }
    }

private:
    std::vector<std::vector<Grid>> grids;  // The 2D array of grids
};