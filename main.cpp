#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

const int windowWidth = 1000;
const int windowHeight = 800;

const int cellQuantityX = 20;
const int cellQuantityY = 20;

sf::Color gridColor(30, 30, 30);
sf::Font font;

struct Ball {
    sf::CircleShape shape;
    //test
};

struct Physics {
    struct Cell {
        struct Entity {
            sf::Vector2f velocity;
            sf::Vector2f position;
            Ball ball;
        };
        sf::Vector2f gridSize;
        sf::Vector2f gridPosition;
        std::vector<Entity> entities;

        // Default constructor
        Cell() : gridSize(0, 0), gridPosition(0, 0) {}

        // Existing constructor
        Cell(float x, float y, float width, float height) {
            gridSize = sf::Vector2f(width, height);
            gridPosition = sf::Vector2f(x, y);
        }

        void addEntity(Ball ball, sf::Vector2f velocity, sf::Vector2f position) {
            //creates an instance of the Entity ball inside correct cell according to position
            entities.push_back({ velocity, position, ball }); 
        }

        bool isInside(sf::Vector2f position) {
            // Check if the position is inside the cell.
            // This is just a placeholder. You need to implement this function.
            return position.x >= gridPosition.x && position.x < gridPosition.x + gridSize.x
                && position.y >= gridPosition.y && position.y < gridPosition.y + gridSize.y;
        }
    };

    std::vector<std::vector<Cell>> cells;

    void applyPositionToEntities(Physics::Cell::Entity& entity) {
        for (auto& row : cells) {
            for (auto& cell : row) {
                for (auto& e : cell.entities) {
                    if (typeid(e.ball) == typeid(entity.ball)) {
                        e.ball.shape.setPosition(e.position);
                    }
                }
            }
        }
    }

    void updateEntities() {
        for (auto& row : cells) {
            for (auto& cell : row) {
                for (auto it = cell.entities.begin(); it != cell.entities.end(); /* no increment here */) {
                    if (!cell.isInside(it->position)) {
                        // Determine the correct cell for the entity based on its position
                        Cell& newCell = determineCell(it->position);
                        // Add the entity to the new cell with the same velocity and position
                        newCell.addEntity(it->ball, it->velocity, it->position);
                        // Remove the entity from the current cell
                        it = cell.entities.erase(it);
                    }
                    else {
                        ++it;
                    }
                }
            }
        }
    }

    Cell& determineCell(sf::Vector2f position) {
        // Get the grid coordinates based on the position and the cell's grid size
        int gridX = static_cast<int>(position.x / cells[0][0].gridSize.x);
        int gridY = static_cast<int>(position.y / cells[0][0].gridSize.y);

        // Ensure the coordinates are within the grid bounds
        gridX = std::max(0, std::min(gridX, static_cast<int>(cells[0].size()) - 1));
        gridY = std::max(0, std::min(gridY, static_cast<int>(cells.size()) - 1));

        // Return a reference to the cell at the calculated coordinates
        return cells[gridY][gridX];
    }


    Physics(int windowSizeX, int windowSizeY, int gridQuantityX, int gridQuantityY) {
        // Calculate the size of each cell
        sf::Vector2f gridSize(windowSizeX / gridQuantityX, windowSizeY / gridQuantityY);

        // Resize the outer vector to have the correct number of rows
        cells.resize(gridQuantityY);

        // For each row...
        for (int y = 0; y < gridQuantityY; ++y) {
            // For each cell in the current row...
            for (int x = 0; x < gridQuantityX; ++x) {
                // Construct the cell with the correct position and size
                cells[y].push_back(Cell(x * gridSize.x, y * gridSize.y, gridSize.x, gridSize.y));
            }
        }
    }
};

struct Debug {
    sf::Font font;
    sf::Text text;

    Debug() {
        // Load the font (you'll need to provide the path to a .ttf file)
        if (!font.loadFromFile("Arial.ttf")) {
            // handle error
        }

        // Initialize the text object
        text.setFont(font);
        text.setCharacterSize(14); // in pixels
        text.setFillColor(sf::Color::White);
    }

    void drawGrid(sf::RenderWindow& window, const std::vector<std::vector<Physics::Cell>>& cells) {
        for (const auto& row : cells) {
            for (const auto& cell : row) {
                sf::RectangleShape rectangle(cell.gridSize);
                rectangle.setPosition(cell.gridPosition);
                rectangle.setOutlineThickness(1);
                rectangle.setOutlineColor(sf::Color::White);
                rectangle.setFillColor(sf::Color::Transparent);
                window.draw(rectangle);
            }
        }
    }

    void drawBallCount(sf::RenderWindow& window, const std::vector<std::vector<Physics::Cell>>& cells) {
        for (const auto& row : cells) {
            for (const auto& cell : row) {
                text.setString(std::to_string(cell.entities.size()));
                text.setPosition(cell.gridPosition);
                window.draw(text);
            }
        }
    }

    void printCellPositions(const std::vector<std::vector<Physics::Cell>>& cells) {
        for (const auto& row : cells) {
            for (const auto& cell : row) {
                std::cout << "Cell at position (" << cell.gridPosition.x << ", " << cell.gridPosition.y << ")\n";
            }
        }
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "My Window");
    Physics Physics(windowWidth, windowHeight, cellQuantityX, cellQuantityY);
    Debug Debug;
    Debug.printCellPositions(Physics.cells);

    while (window.isOpen()) {
        // Process events
        Debug.drawGrid(window, Physics.cells);
        Debug.drawBallCount(window, Physics.cells);
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.display();
    }

    return 0;
}