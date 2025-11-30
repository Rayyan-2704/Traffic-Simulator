#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "../include/Graph.h"
#include "../include/Vehicle.h"
#include "../include/TrafficSignal.h"
using namespace std;

int main()
{
    cout << "\n=== Traffic Simulator - Day 4: Traffic Signals + Queues ===\n"
         << endl;

    // Load city map
    Graph cityMap;
    if (!cityMap.loadFromFile("data/roads.txt"))
    {
        cout << "Failed to load map!" << endl;
        return 1;
    }
    cout << "City loaded: " << cityMap.getNumNodes() << " nodes\n"
         << endl;

    // Create window
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Traffic Simulator - Day 4");
    window.setFramerateLimit(60);

    // Load font
    sf::Font font;
    bool fontLoaded = font.openFromFile("assets/font.ttf");

    // Load car textures (Kenney Racing Pack)
    vector<sf::Texture> carTextures(5);
    vector<bool> carTextureLoaded(5, false);

    vector<string> carFiles = {
        "assets/cars/car_blue_1.png",
        "assets/cars/car_red_1.png",
        "assets/cars/car_green_1.png",
        "assets/cars/car_yellow_1.png",
        "assets/cars/car_black_1.png"};

    for (int i = 0; i < 5; i++)
    {
        carTextureLoaded[i] = carTextures[i].loadFromFile(carFiles[i]);
        if (carTextureLoaded[i])
            cout << "Loaded: " << carFiles[i] << endl;
    }

    // Load traffic light sprites (Kenney Racing Pack)
    sf::Texture trafficLightRed, trafficLightGreen;
    bool redLightLoaded = trafficLightRed.loadFromFile("assets/signals/trafficLight_red.png");
    bool greenLightLoaded = trafficLightGreen.loadFromFile("assets/signals/trafficLight_green.png");

    if (redLightLoaded && greenLightLoaded)
        cout << "Traffic light sprites loaded successfully!\n"
             << endl;

    // Create traffic signals at key intersections
    vector<TrafficSignal> signals;

    // Signal 1: At node 1 (manual control)
    signals.push_back(TrafficSignal(1, 1, cityMap.getNode(1).position, 8.0f, 6.0f, false));

    // Signal 2: At node 5 (automatic)
    signals.push_back(TrafficSignal(2, 5, cityMap.getNode(5).position, 7.0f, 5.0f, true));

    // Signal 3: At node 6 (manual control)
    signals.push_back(TrafficSignal(3, 6, cityMap.getNode(6).position, 6.0f, 6.0f, false));

    // Signal 4: At node 9 (automatic)
    signals.push_back(TrafficSignal(4, 9, cityMap.getNode(9).position, 5.0f, 7.0f, true));

    cout << "Created " << signals.size() << " traffic signals\n"
         << endl;

    // Spawn vehicles
    vector<Vehicle> cars;

    // Car 1: 0 -> 11
    vector<int> path1 = cityMap.dijkstraAlgorithm(0, 11);
    Vehicle car1(1, path1, 1.0);
    car1.setPosition(cityMap.getNode(0).position);
    car1.setStartPosition(cityMap.getNode(0).position);
    if (path1.size() > 1)
        car1.setTargetPosition(cityMap.getNode(path1[1]).position);
    cars.push_back(car1);

    // Car 2: 2 -> 10
    vector<int> path2 = cityMap.dijkstraAlgorithm(2, 10);
    Vehicle car2(2, path2, 0.8);
    car2.setPosition(cityMap.getNode(2).position);
    car2.setStartPosition(cityMap.getNode(2).position);
    if (path2.size() > 1)
        car2.setTargetPosition(cityMap.getNode(path2[1]).position);
    cars.push_back(car2);

    // Car 3: 0 -> 7
    vector<int> path3 = cityMap.dijkstraAlgorithm(0, 7);
    Vehicle car3(3, path3, 1.2);
    car3.setPosition(cityMap.getNode(0).position);
    car3.setStartPosition(cityMap.getNode(0).position);
    if (path3.size() > 1)
        car3.setTargetPosition(cityMap.getNode(path3[1]).position);
    cars.push_back(car3);

    // Car 4: 4 -> 11
    vector<int> path4 = cityMap.dijkstraAlgorithm(4, 11);
    Vehicle car4(4, path4, 0.9);
    car4.setPosition(cityMap.getNode(4).position);
    car4.setStartPosition(cityMap.getNode(4).position);
    if (path4.size() > 1)
        car4.setTargetPosition(cityMap.getNode(path4[1]).position);
    cars.push_back(car4);

    // Car 5: 3 -> 8
    vector<int> path5 = cityMap.dijkstraAlgorithm(3, 8);
    Vehicle car5(5, path5, 1.1);
    car5.setPosition(cityMap.getNode(3).position);
    car5.setStartPosition(cityMap.getNode(3).position);
    if (path5.size() > 1)
        car5.setTargetPosition(cityMap.getNode(path5[1]).position);
    cars.push_back(car5);

    cout << "Spawned " << cars.size() << " vehicles\n"
         << endl;

    // Print controls
    cout << "=== CONTROLS ===" << endl;
    cout << "1 - Toggle Signal 1 (Node 1)" << endl;
    cout << "2 - Toggle Signal 2 (Node 5)" << endl;
    cout << "3 - Toggle Signal 3 (Node 6)" << endl;
    cout << "4 - Toggle Signal 4 (Node 9)" << endl;
    cout << "ESC - Exit simulation" << endl;
    cout << "================\n"
         << endl;

    // Clock for delta time
    sf::Clock clock;

    // Main loop
    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();

        // Event handling
        while (const optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            // Handle key presses for toggling signals
            if (event->is<sf::Event::KeyPressed>())
            {
                auto keyEvent = event->getIf<sf::Event::KeyPressed>();

                if (keyEvent->code == sf::Keyboard::Key::Num1)
                    signals[0].toggleLight();
                else if (keyEvent->code == sf::Keyboard::Key::Num2)
                    signals[1].toggleLight();
                else if (keyEvent->code == sf::Keyboard::Key::Num3)
                    signals[2].toggleLight();
                else if (keyEvent->code == sf::Keyboard::Key::Num4)
                    signals[3].toggleLight();
                else if (keyEvent->code == sf::Keyboard::Key::Escape)
                    window.close();
            }
        }

        // Update automatic signals
        for (auto &signal : signals)
        {
            if (signal.isAutoMode())
                signal.updateTimer(deltaTime);
        }

        // Update vehicles
        for (auto &car : cars)
        {
            if (car.hasArrivedDest())
                continue;

            if (car.canMove())
            {
                car.updateVisualPosition(deltaTime);

                // Check if we finished the current segment (reached current node)
                if (car.getInterpolation() >= 1.0f)
                {
                    int currentNodeId = car.getCurrentNode();
                    int nextNodeId = car.getNextNode();

                    if (nextNodeId != -1)
                    {
                        // Check if CURRENT node (where car is now) has a red signal
                        bool blockedBySignal = false;
                        for (auto& signal : signals)
                        {
                            if (signal.getIntersectionID() == currentNodeId && 
                                signal.getState() == SignalState::RED)
                            {
                                signal.addVehicleToQueue(&car);
                                blockedBySignal = true;
                                cout << "Vehicle " << car.getID() << " stopped at Node " 
                                     << currentNodeId << " (RED signal)" << endl;
                                break;
                            }
                        }

                        // Only move to next node if not blocked
                        if (!blockedBySignal)
                        {
                            sf::Vector2<float> nextPos = cityMap.getNode(nextNodeId).position;
                            car.moveToNextNode(nextPos);
                        }
                    }
                    else
                    {
                        car.setStatus(VehicleStatus::ARRIVED);
                        cout << "Vehicle " << car.getID() << " arrived at destination!" << endl;
                    }
                }
            }
        }

        // Clear window
        window.clear(sf::Color(30, 30, 30));

        // --- DRAW ROADS ---
        for (const auto &nodePair : cityMap.getAllNodes())
        {
            int nodeId = nodePair.first;
            const Node &node = nodePair.second;

            for (const Edge &edge : cityMap.getNeighbours(nodeId))
            {
                const Node &neighbor = cityMap.getNode(edge.destination);
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

            sf::CircleShape intersection(12);
            intersection.setPosition(node.position - sf::Vector2<float>(12, 12));
            intersection.setFillColor(sf::Color(200, 200, 200));
            intersection.setOutlineThickness(2);
            intersection.setOutlineColor(sf::Color::White);
            window.draw(intersection);

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

        // --- DRAW TRAFFIC SIGNALS (WITH SPRITES!) ---
        for (const auto &signal : signals)
        {
            sf::Vector2<float> sigPos = signal.getPosition();

            // Choose texture based on signal state
            bool usedSprite = false;

            if (signal.getState() == SignalState::RED && redLightLoaded)
            {
                sf::Sprite lightSprite(trafficLightRed);
                lightSprite.setScale({0.4f, 0.4f});
                lightSprite.setPosition(sigPos + sf::Vector2<float>(15, -30));
                window.draw(lightSprite);
                usedSprite = true;
            }
            else if (signal.getState() == SignalState::GREEN && greenLightLoaded)
            {
                sf::Sprite lightSprite(trafficLightGreen);
                lightSprite.setScale({0.4f, 0.4f});
                lightSprite.setPosition(sigPos + sf::Vector2<float>(15, -30));
                window.draw(lightSprite);
                usedSprite = true;
            }

            // Fallback to colored circle if sprite not loaded
            if (!usedSprite)
            {
                sf::CircleShape signalCircle(8);
                signalCircle.setPosition(sigPos + sf::Vector2<float>(20, -20));
                signalCircle.setFillColor(signal.getColor());
                signalCircle.setOutlineThickness(2);
                signalCircle.setOutlineColor(sf::Color::Black);
                window.draw(signalCircle);
            }

            // Draw signal ID label
            if (fontLoaded)
            {
                sf::Text sigLabel(font);
                sigLabel.setString("S" + to_string(signal.getID()));
                sigLabel.setCharacterSize(10);
                sigLabel.setFillColor(sf::Color::White);
                sigLabel.setOutlineColor(sf::Color::Black);
                sigLabel.setOutlineThickness(1);
                sigLabel.setPosition(sigPos + sf::Vector2<float>(25, -5));
                window.draw(sigLabel);

                // Show queue size if > 0
                if (signal.getQueueSize() > 0)
                {
                    sf::Text queueLabel(font);
                    queueLabel.setString("Q:" + to_string(signal.getQueueSize()));
                    queueLabel.setCharacterSize(12);
                    queueLabel.setFillColor(sf::Color::Yellow);
                    queueLabel.setOutlineColor(sf::Color::Black);
                    queueLabel.setOutlineThickness(1);
                    queueLabel.setPosition(sigPos + sf::Vector2<float>(20, 8));
                    window.draw(queueLabel);
                }
            }
        }

        // --- DRAW VEHICLES (WITH SPRITES!) ---
        for (int i = 0; i < cars.size(); i++)
        {
            const auto &car = cars[i];
            if (!car.hasArrivedDest())
            {
                sf::Vector2<float> carPos = car.getPosition();

                if (carTextureLoaded[i % carTextures.size()])
                {
                    // Draw as Kenney sprite
                    sf::Sprite carSprite(carTextures[i % carTextures.size()]);

                    // Center the sprite
                    sf::FloatRect bounds = carSprite.getLocalBounds();
                    carSprite.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
                    carSprite.setPosition(carPos);
                    carSprite.setScale({0.35f, 0.35f});

                    // Tint waiting cars yellow
                    if (car.getStatus() == VehicleStatus::WAITING)
                        carSprite.setColor(sf::Color(255, 255, 150));
                    else
                        carSprite.setColor(sf::Color::White);

                    window.draw(carSprite);
                }
                else
                {
                    // Fallback to colored rectangles
                    sf::RectangleShape carRect({20, 12});
                    carRect.setPosition(carPos - sf::Vector2<float>(10, 6));

                    // Color based on status
                    if (car.getStatus() == VehicleStatus::WAITING)
                        carRect.setFillColor(sf::Color::Yellow);
                    else
                    {
                        sf::Color colors[] = {sf::Color::Red, sf::Color::Blue,
                                              sf::Color::Green, sf::Color::Cyan,
                                              sf::Color::Magenta};
                        carRect.setFillColor(colors[i % 5]);
                    }

                    window.draw(carRect);
                }
            }
        }

        // --- DRAW HUD ---
        if (fontLoaded)
        {
            int moving = 0, waiting = 0, arrived = 0;
            for (const auto &car : cars)
            {
                if (car.hasArrivedDest())
                    arrived++;
                else if (car.getStatus() == VehicleStatus::WAITING)
                    waiting++;
                else
                    moving++;
            }

            sf::Text hud(font);
            string hudText = "Moving: " + to_string(moving) + " | Waiting: " +
                             to_string(waiting) + " | Arrived: " + to_string(arrived);
            hud.setString(hudText);
            hud.setCharacterSize(18);
            hud.setFillColor(sf::Color::White);
            hud.setPosition({10, 10});
            window.draw(hud);

            // Controls reminder
            sf::Text controls(font);
            controls.setString("Press 1-4 to toggle signals | ESC to exit");
            controls.setCharacterSize(14);
            controls.setFillColor(sf::Color(180, 180, 180));
            controls.setPosition({10, 570});
            window.draw(controls);
        }

        window.display();
    }

    // Print final stats
    cout << "\n=== FINAL STATISTICS ===" << endl;
    for (const auto &signal : signals)
    {
        signal.printInfo();
    }

    cout << "\n=== Simulation Ended ===" << endl;
    return 0;
}