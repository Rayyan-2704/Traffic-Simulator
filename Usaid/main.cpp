#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "../include/Graph.h"
#include "../include/Vehicle.h"
#include "../include/TrafficSignal.h"
#include "../include/EmergencyVehicle.h"
#include "../include/ActionStack.h"
#include "../include/PriorityQueue.h"
using namespace std;

int main()
{
    cout << "\n=== Traffic Simulator - Days 6-7: Stack & Priority Queue ===\n"
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
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Traffic Simulator - Stack & Priority Queue");
    window.setFramerateLimit(60);

    // Load font
    sf::Font font;
    bool fontLoaded = font.openFromFile("assets/font.ttf");

    // Load car textures
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

    // Load traffic light sprites
    sf::Texture trafficLightRed, trafficLightGreen;
    bool redLightLoaded = trafficLightRed.loadFromFile("assets/signals/trafficLight_red.png");
    bool greenLightLoaded = trafficLightGreen.loadFromFile("assets/signals/trafficLight_green.png");

    if (redLightLoaded && greenLightLoaded)
        cout << "Traffic light sprites loaded successfully!\n"
             << endl;

    // Load ambulance texture
    sf::Texture ambulanceTexture;
    bool ambulanceLoaded = ambulanceTexture.loadFromFile("assets/cars/ambulance.png");
    if (ambulanceLoaded)
        cout << "Ambulance texture loaded!\n"
             << endl;

    // Create traffic signals
    vector<TrafficSignal> signals;
    signals.push_back(TrafficSignal(1, 1, cityMap.getNode(1).position, 8.0f, 6.0f, false));
    signals.push_back(TrafficSignal(2, 5, cityMap.getNode(5).position, 7.0f, 5.0f, true));
    signals.push_back(TrafficSignal(3, 6, cityMap.getNode(6).position, 6.0f, 6.0f, false));
    signals.push_back(TrafficSignal(4, 9, cityMap.getNode(9).position, 5.0f, 7.0f, true));

    cout << "Created " << signals.size() << " traffic signals\n"
         << endl;

    // Initialize ActionStack for undo system
    ActionStack undoStack(50);

    // Initialize Priority Queue for emergency vehicles
    PriorityQueue<EmergencyVehicle *> emergencyQueue;

    // Regular vehicles
    vector<Vehicle> cars;
    int nextCarID = 1;

    // Spawn initial vehicles
    vector<int> path1 = cityMap.dijkstraAlgorithm(0, 11);
    Vehicle car1(nextCarID++, path1, 1.0);
    car1.setPosition(cityMap.getNode(0).position);
    car1.setStartPosition(cityMap.getNode(0).position);
    if (path1.size() > 1)
        car1.setTargetPosition(cityMap.getNode(path1[1]).position);
    cars.push_back(car1);

    vector<int> path2 = cityMap.dijkstraAlgorithm(2, 10);
    Vehicle car2(nextCarID++, path2, 0.8);
    car2.setPosition(cityMap.getNode(2).position);
    car2.setStartPosition(cityMap.getNode(2).position);
    if (path2.size() > 1)
        car2.setTargetPosition(cityMap.getNode(path2[1]).position);
    cars.push_back(car2);

    cout << "Spawned " << cars.size() << " regular vehicles\n"
         << endl;

    // Emergency vehicles storage
    vector<EmergencyVehicle *> emergencyVehicles;
    int nextEmergencyID = 100;

    // Print controls
    cout << "=== CONTROLS ===" << endl;
    cout << "1-4 - Toggle Signals" << endl;
    cout << "A   - Add Regular Car" << endl;
    cout << "E   - Spawn Emergency Vehicle (Ambulance)" << endl;
    cout << "U   - Undo Last Action" << endl;
    cout << "P   - Pause/Resume Simulation" << endl;
    cout << "ESC - Exit" << endl;
    cout << "================\n"
         << endl;

    sf::Clock clock;
    bool paused = false;

    // Main loop
    while (window.isOpen())
    {
        float deltaTime = paused ? 0.0f : clock.restart().asSeconds();

        // Event handling
        while (const optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::KeyPressed>())
            {
                auto keyEvent = event->getIf<sf::Event::KeyPressed>();

                // Toggle signals (with undo recording)
                if (keyEvent->code == sf::Keyboard::Key::Num1)
                {
                    SignalState prevState = signals[0].getState();
                    signals[0].toggleLight();
                    undoStack.recordAction(Action(ActionType::TOGGLE_SIGNAL, 1, prevState, "Toggled Signal 1"));
                }
                else if (keyEvent->code == sf::Keyboard::Key::Num2)
                {
                    SignalState prevState = signals[1].getState();
                    signals[1].toggleLight();
                    undoStack.recordAction(Action(ActionType::TOGGLE_SIGNAL, 2, prevState, "Toggled Signal 2"));
                }
                else if (keyEvent->code == sf::Keyboard::Key::Num3)
                {
                    SignalState prevState = signals[2].getState();
                    signals[2].toggleLight();
                    undoStack.recordAction(Action(ActionType::TOGGLE_SIGNAL, 3, prevState, "Toggled Signal 3"));
                }
                else if (keyEvent->code == sf::Keyboard::Key::Num4)
                {
                    SignalState prevState = signals[3].getState();
                    signals[3].toggleLight();
                    undoStack.recordAction(Action(ActionType::TOGGLE_SIGNAL, 4, prevState, "Toggled Signal 4"));
                }
                // Add regular car
                else if (keyEvent->code == sf::Keyboard::Key::A)
                {
                    int start = rand() % cityMap.getNumNodes();
                    int end = rand() % cityMap.getNumNodes();
                    while (start == end)
                        end = rand() % cityMap.getNumNodes();

                    vector<int> path = cityMap.dijkstraAlgorithm(start, end);
                    if (!path.empty())
                    {
                        Vehicle newCar(nextCarID, path, 1.0);
                        newCar.setPosition(cityMap.getNode(start).position);
                        newCar.setStartPosition(cityMap.getNode(start).position);
                        if (path.size() > 1)
                            newCar.setTargetPosition(cityMap.getNode(path[1]).position);

                        cars.push_back(newCar);
                        undoStack.recordAction(Action(ActionType::ADD_VEHICLE, nextCarID, "Added Car " + to_string(nextCarID)));
                        cout << "Added Car " << nextCarID << " from Node " << start << " to " << end << endl;
                        nextCarID++;
                    }
                }
                // Spawn emergency vehicle
                else if (keyEvent->code == sf::Keyboard::Key::E)
                {
                    int start = rand() % cityMap.getNumNodes();
                    int end = rand() % cityMap.getNumNodes();
                    while (start == end)
                        end = rand() % cityMap.getNumNodes();

                    vector<int> path = cityMap.dijkstraAlgorithm(start, end);
                    if (!path.empty())
                    {
                        EmergencyVehicle *ambulance = new EmergencyVehicle(
                            nextEmergencyID++, path, EmergencyType::AMBULANCE, 2.0);

                        ambulance->setPosition(cityMap.getNode(start).position);
                        ambulance->setStartPosition(cityMap.getNode(start).position);
                        if (path.size() > 1)
                            ambulance->setTargetPosition(cityMap.getNode(path[1]).position);

                        emergencyQueue.push(ambulance);
                        emergencyVehicles.push_back(ambulance);

                        cout << "Emergency Vehicle " << ambulance->getID() << " spawned! Priority Queue size: "
                             << emergencyQueue.size() << endl;
                    }
                }
                // Undo last action
                else if (keyEvent->code == sf::Keyboard::Key::U)
                {
                    if (undoStack.canUndo())
                    {
                        Action lastAction = undoStack.getLastAction();
                        cout << "Undoing: " << lastAction.description << endl;

                        if (lastAction.type == ActionType::TOGGLE_SIGNAL)
                        {
                            // Find the signal and revert its state
                            for (auto &signal : signals)
                            {
                                if (signal.getID() == lastAction.targetID)
                                {
                                    signal.setState(lastAction.previousState);
                                    cout << "Signal " << lastAction.targetID << " reverted to previous state" << endl;
                                    break;
                                }
                            }
                        }
                        else if (lastAction.type == ActionType::ADD_VEHICLE)
                        {
                            // Remove the last added vehicle
                            for (auto it = cars.begin(); it != cars.end(); ++it)
                            {
                                if (it->getID() == lastAction.targetID)
                                {
                                    cout << "Removed Car " << lastAction.targetID << endl;
                                    cars.erase(it);
                                    break;
                                }
                            }
                        }

                        undoStack.popLastAction();
                    }
                    else
                    {
                        cout << "Nothing to undo!" << endl;
                    }
                }
                // Pause/Resume
                else if (keyEvent->code == sf::Keyboard::Key::P)
                {
                    paused = !paused;
                    cout << (paused ? "Simulation PAUSED" : "Simulation RESUMED") << endl;
                }
                else if (keyEvent->code == sf::Keyboard::Key::Escape)
                    window.close();
            }
        }

        if (!paused)
        {
            // Update automatic signals
            for (auto &signal : signals)
            {
                if (signal.isAutoMode())
                    signal.updateTimer(deltaTime);
            }

            // Process emergency vehicles first (highest priority)
            if (!emergencyQueue.empty())
            {
                EmergencyVehicle *priorityVehicle = emergencyQueue.top();

                if (!priorityVehicle->hasArrivedDest() && priorityVehicle->canMove())
                {
                    priorityVehicle->updateVisualPosition(deltaTime);

                    if (priorityVehicle->getInterpolation() >= 1.0f)
                    {
                        int nextNode = priorityVehicle->getNextNode();
                        if (nextNode != -1)
                        {
                            sf::Vector2<float> nextPos = cityMap.getNode(nextNode).position;
                            priorityVehicle->moveToNextNode(nextPos);
                        }
                        else
                        {
                            priorityVehicle->setStatus(VehicleStatus::ARRIVED);
                            cout << "Emergency Vehicle " << priorityVehicle->getID() << " arrived!" << endl;
                            emergencyQueue.pop();
                        }
                    }
                }
            }

            // Update regular vehicles
            for (auto &car : cars)
            {
                if (car.hasArrivedDest())
                    continue;

                if (car.canMove())
                {
                    car.updateVisualPosition(deltaTime);

                    if (car.getInterpolation() >= 1.0f)
                    {
                        int currentNodeId = car.getCurrentNode();
                        int nextNodeId = car.getNextNode();

                        if (nextNodeId != -1)
                        {
                            bool blockedBySignal = false;
                            for (auto &signal : signals)
                            {
                                if (signal.getIntersectionID() == currentNodeId &&
                                    signal.getState() == SignalState::RED)
                                {
                                    signal.addVehicleToQueue(&car);
                                    blockedBySignal = true;
                                    break;
                                }
                            }

                            if (!blockedBySignal)
                            {
                                sf::Vector2<float> nextPos = cityMap.getNode(nextNodeId).position;
                                car.moveToNextNode(nextPos);
                            }
                        }
                        else
                        {
                            car.setStatus(VehicleStatus::ARRIVED);
                        }
                    }
                }
            }
        }

        // Rendering
        window.clear(sf::Color(30, 30, 30));

        // Draw roads
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

        // Draw intersections
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

        // Draw traffic signals
        for (const auto &signal : signals)
        {
            sf::Vector2<float> sigPos = signal.getPosition();
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

            if (!usedSprite)
            {
                sf::CircleShape signalCircle(8);
                signalCircle.setPosition(sigPos + sf::Vector2<float>(20, -20));
                signalCircle.setFillColor(signal.getColor());
                signalCircle.setOutlineThickness(2);
                signalCircle.setOutlineColor(sf::Color::Black);
                window.draw(signalCircle);
            }

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

        // Draw emergency vehicles (with red flashing effect)
        for (auto *emergencyVehicle : emergencyVehicles)
        {
            if (!emergencyVehicle->hasArrivedDest())
            {
                sf::Vector2<float> carPos = emergencyVehicle->getPosition();

                if (ambulanceLoaded)
                {
                    sf::Sprite ambulanceSprite(ambulanceTexture);
                    sf::FloatRect bounds = ambulanceSprite.getLocalBounds();
                    ambulanceSprite.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
                    ambulanceSprite.setPosition(carPos);
                    ambulanceSprite.setScale({0.4f, 0.4f});
                    window.draw(ambulanceSprite);
                }
                else
                {
                    sf::RectangleShape emergencyRect({25, 15});
                    emergencyRect.setPosition(carPos - sf::Vector2<float>(12.5f, 7.5f));
                    emergencyRect.setFillColor(sf::Color::Red);
                    emergencyRect.setOutlineThickness(2);
                    emergencyRect.setOutlineColor(sf::Color::White);
                    window.draw(emergencyRect);
                }
            }
        }

        // Draw regular vehicles
        for (int i = 0; i < cars.size(); i++)
        {
            const auto &car = cars[i];
            if (!car.hasArrivedDest())
            {
                sf::Vector2<float> carPos = car.getPosition();

                if (carTextureLoaded[i % carTextures.size()])
                {
                    sf::Sprite carSprite(carTextures[i % carTextures.size()]);
                    sf::FloatRect bounds = carSprite.getLocalBounds();
                    carSprite.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
                    carSprite.setPosition(carPos);
                    carSprite.setScale({0.35f, 0.35f});

                    if (car.getStatus() == VehicleStatus::WAITING)
                        carSprite.setColor(sf::Color(255, 255, 150));
                    else
                        carSprite.setColor(sf::Color::White);

                    window.draw(carSprite);
                }
                else
                {
                    sf::RectangleShape carRect({20, 12});
                    carRect.setPosition(carPos - sf::Vector2<float>(10, 6));

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

        // Draw HUD
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
            string hudText = "Cars: " + to_string(moving) + " | Waiting: " +
                             to_string(waiting) + " | Emergency: " + to_string(emergencyQueue.size()) +
                             " | Undo: " + to_string(undoStack.getStackSize());
            hud.setString(hudText);
            hud.setCharacterSize(16);
            hud.setFillColor(sf::Color::White);
            hud.setPosition({10, 10});
            window.draw(hud);

            if (paused)
            {
                sf::Text pauseText(font);
                pauseText.setString("PAUSED");
                pauseText.setCharacterSize(30);
                pauseText.setFillColor(sf::Color::Yellow);
                pauseText.setOutlineColor(sf::Color::Black);
                pauseText.setOutlineThickness(2);
                pauseText.setPosition({350, 280});
                window.draw(pauseText);
            }

            sf::Text controls(font);
            controls.setString("1-4: Signals | A: Add Car | E: Emergency | U: Undo | P: Pause | ESC: Exit");
            controls.setCharacterSize(12);
            controls.setFillColor(sf::Color(180, 180, 180));
            controls.setPosition({10, 575});
            window.draw(controls);
        }

        window.display();
    }

    // Cleanup
    for (auto *emergencyVehicle : emergencyVehicles)
    {
        delete emergencyVehicle;
    }

    cout << "\n=== Simulation Ended ===" << endl;
    cout << "Final Undo Stack Size: " << undoStack.getStackSize() << endl;
    return 0;
}
