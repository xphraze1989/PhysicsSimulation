#include <SFML/Graphics.hpp>
#include <vector>

const int windowWidth = 1000;
const int windowHeight = 800;

const int cellQuantityX = 10;
const int cellQuantityY = 10;

sf::Color gridColor(30, 30, 30);
sf::Font font;

struct Ball {
    sf::CircleShape shape;

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

        Cell(float x, float y, float width, float height) {
            //takes const int windowWidth and windowHeight and sets right size and position for the cell.
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
        //takes windowwidth and windowhight and calculates the borders of the simulation
        //it also creates instances of the grid with the right position on screen.
    }

    Physics(int windowSizeX, int windowSizeY) {
        //takes windowwidth and windowhight and calculates the borders of the simulation only
    }

    // Rest of the Physics struct...
};

struct Debug {
    // Debugging code goes here...
};

void setup(sf::RenderWindow& window) {
    // Setup code goes here...
    // For example, you might initialize some objects or load some resources.
}

void loop(sf::RenderWindow& window) {
    // Main loop code goes here...
    // This is where you would handle events, update your objects, and draw them.
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "My Window");
    setup(window);

    while (window.isOpen()) {
        // Process events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        loop(window);
        window.display();
    }

    return 0;
}