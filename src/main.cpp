#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "../include/Graph.h"
#include "../include/Vehicle.h"
using namespace std;

int main()
{
    cout << "=== Traffic Simulator - Day 3: SFML 3.0 Visualization ===" << endl;

    // Load city map
    Graph cityMap;
    if (!cityMap.loadFromFile("data/roads.txt"))
    {
        cout << "Failed to load map!" << endl;
        return 1;
    }
    cout << "City loaded: " << cityMap.getNumNodes() << " nodes" << endl;

    // Create window (SFML 3.0 syntax)
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Traffic Simulator - SFML 3.0");
    window.setFramerateLimit(60);

    // Load font (optional - for node labels)
    sf::Font font;
    bool fontLoaded = false;
    if (font.openFromFile("assets/font.ttf"))
        fontLoaded = true;

    // Load car texture
    vector<sf::Texture> carTextures(3); // 3 cars
    vector<bool> textureLoaded(3, false);
    if (carTextures[0].loadFromFile("assets/car1.png"))
        textureLoaded[0] = true;
    if (carTextures[1].loadFromFile("assets/car2.png"))
        textureLoaded[1] = true;
    if (carTextures[2].loadFromFile("assets/car3.png"))
        textureLoaded[2] = true;

    // Spawn vehicles
    vector<Vehicle> cars;

    // Car 1: 0 -> 7
    vector<int> path1 = cityMap.dijkstraAlgorithm(0, 7);
    Vehicle car1(1, path1, 1.0);
    car1.setPosition(cityMap.getNode(0).position);
    car1.setStartPosition(cityMap.getNode(0).position);
    if (path1.size() > 1)
    {
        car1.setTargetPosition(cityMap.getNode(path1[1]).position); // Set initial target
    }
    cars.push_back(car1);

    // Car 2: 3 -> 11
    vector<int> path2 = cityMap.dijkstraAlgorithm(3, 11);
    Vehicle car2(2, path2, 0.8);
    car2.setPosition(cityMap.getNode(3).position);
    car2.setStartPosition(cityMap.getNode(3).position);
    if (path2.size() > 1)
    {
        car2.setTargetPosition(cityMap.getNode(path2[1]).position); // Set initial target
    }
    cars.push_back(car2);

    // Car 3: 0 -> 11
    vector<int> path3 = cityMap.dijkstraAlgorithm(0, 11);
    Vehicle car3(3, path3, 1.2);
    car3.setPosition(cityMap.getNode(0).position);
    car3.setStartPosition(cityMap.getNode(0).position);
    if (path3.size() > 1)
    {
        car3.setTargetPosition(cityMap.getNode(path3[1]).position); // Set initial target
    }
    cars.push_back(car3);

    cout << "Spawned " << cars.size() << " vehicles" << endl;
    cout << "Controls: ESC to exit" << endl;

    // Clock for delta time
    sf::Clock clock;

    // Main loop
    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // Check for Escape key
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
        {
            window.close();
        }

        // Update vehicles
        for (auto &car : cars)
        {
            if (!car.hasArrivedDest())
            {
                // Update visual position (moves toward target)
                car.updateVisualPosition(deltaTime);

                // Check if we finished the current segment
                if (car.getInterpolation() >= 1.0f)
                {
                    int nextNodeId = car.getNextNode();

                    if (nextNodeId != -1)
                    {
                        // Set next target node
                        sf::Vector2<float> nextPos = cityMap.getNode(nextNodeId).position;
                        car.moveToNextNode(nextPos);
                    }
                    else
                    {
                        // No more nodes left; we are at the final destination
                        car.setStatus(VehicleStatus::ARRIVED);
                        cout << "Vehicle " << car.getID() << " has arrived at destination node " << car.getDestNode() << endl;
                    }
                }
            }
        }

        // Clear
        window.clear(sf::Color(30, 30, 30)); // Dark gray background

        // --- DRAW ROADS ---
        for (const auto &nodePair : cityMap.getAllNodes())
        {
            int nodeId = nodePair.first;
            const Node &node = nodePair.second;

            for (const Edge &edge : cityMap.getNeighbours(nodeId))
            {
                const Node &neighbor = cityMap.getNode(edge.destination);

                // Draw line from node to neighbor
                sf::Vertex line[] = {
                    sf::Vertex{node.position, sf::Color(100, 100, 100)},
                    sf::Vertex{neighbor.position, sf::Color(100, 100, 100)}};
                window.draw(line, 2, sf::PrimitiveType::Lines);
            }
        }

        // --- DRAW INTERSECTIONS ---
        for (const auto &nodePair : cityMap.getAllNodes())
        {
            const Node &node = nodePair.second;

            // Draw circle
            sf::CircleShape intersection(12);
            intersection.setPosition(node.position - sf::Vector2<float>(12, 12));
            intersection.setFillColor(sf::Color(200, 200, 200));
            intersection.setOutlineThickness(2);
            intersection.setOutlineColor(sf::Color::White);
            window.draw(intersection);

            // Draw label
            if (fontLoaded)
            {
                sf::Text label(font);
                label.setString(to_string(node.ID));
                label.setCharacterSize(14);
                label.setFillColor(sf::Color::Black);
                label.setPosition(node.position - sf::Vector2<float>(5, 8));
                window.draw(label);
            }
        }

        // --- DRAW VEHICLES ---
        for (int i = 0; i < cars.size(); i++)
        {
            const auto &car = cars[i];
            if (!car.hasArrivedDest())
            {
                sf::Vector2<float> carPos = car.getPosition();

                if (textureLoaded[i])
                {
                    // Draw as sprite
                    sf::Sprite carSprite(carTextures[i]);
                    carSprite.setPosition(carPos - sf::Vector2<float>(16, 16));
                    carSprite.setScale({0.5f, 0.5f});
                    window.draw(carSprite);
                }
                else
                {
                    // Draw as colored rectangle (fallback)
                    sf::RectangleShape carRect({20, 12});
                    carRect.setPosition(carPos - sf::Vector2<float>(10, 6));

                    // Different colors for different cars
                    if (car.getID() == 1)
                        carRect.setFillColor(sf::Color::Red);
                    else if (car.getID() == 2)
                        carRect.setFillColor(sf::Color::Blue);
                    else
                        carRect.setFillColor(sf::Color::Green);

                    window.draw(carRect);
                }
            }
        }

        // --- DRAW HUD ---
        if (fontLoaded)
        {
            int moving = 0, arrived = 0;
            for (const auto &car : cars)
            {
                if (car.hasArrivedDest())
                    arrived++;
                else
                    moving++;
            }

            sf::Text hud(font);
            hud.setString("Cars Moving: " + to_string(moving) + " | Arrived: " + to_string(arrived));
            hud.setCharacterSize(18);
            hud.setFillColor(sf::Color::White);
            hud.setPosition({10, 10});
            window.draw(hud);
        }

        // Display
        window.display();
    }

    cout << "\n=== INITIAL VEHICLE INFO ===" << endl;
    for (auto &car : cars)
    {
        car.printInfo();
    }

    cout << "\n=== Simulation Ended ===" << endl;
    return 0;
}