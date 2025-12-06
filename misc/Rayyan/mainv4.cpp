#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "../include/Graph.h"
#include "../include/Vehicle.h"
#include "../include/TrafficSignal.h"
#include "../include/ActionStack.h"
#include "../include/EmergencyManager.h"
using namespace std;

bool hasSignal(const vector<TrafficSignal>& signals, int nodeID)
{
    for (const auto &signal : signals)
    {
        if(signal.getIntersectionID() == nodeID) return true;
    }

    return false;
}

int main()
{
    cout << "\n=== Traffic Simulator - Day 5: Stack + Priority Queue ===\n" << endl;

    srand(time(nullptr)); // Seed random for emergency spawning

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
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Traffic Simulator - Day 5");
    window.setFramerateLimit(60);

    // Load font
    sf::Font font;
    bool fontLoaded = font.openFromFile("assets/font.ttf");

    // Load car textures (regular)
    vector<sf::Texture> carTextures(5);
    vector<bool> carTextureLoaded(5, false);

    vector<string> carFiles = {
        "assets/images/vehicles/car_blue.png",
        "assets/images/vehicles/car_red.png",
        "assets/images/vehicles/car_green.png",
        "assets/images/vehicles/car_yellow.png",
        "assets/images/vehicles/car_black.png"};

    for (int i = 0; i < 5; i++)
    {
        carTextureLoaded[i] = carTextures[i].loadFromFile(carFiles[i]);
        if (carTextureLoaded[i])
            cout << "Loaded: " << carFiles[i] << endl;
    }

    // Load emergency vehicle textures
    sf::Texture ambulanceTexture, policeTexture;
    bool ambulanceLoaded = ambulanceTexture.loadFromFile("assets/images/vehicles/ambulance.png");
    bool policeLoaded = policeTexture.loadFromFile("assets/images/vehicles/police.png");

    if (ambulanceLoaded && policeLoaded)
        cout << "Emergency vehicle sprites loaded!\n"
             << endl;

    // Load traffic light sprites
    sf::Texture trafficLightRed, trafficLightGreen;
    bool redLightLoaded = trafficLightRed.loadFromFile("assets/images/signals/trafficLight_red.png");
    bool greenLightLoaded = trafficLightGreen.loadFromFile("assets/images/signals/trafficLight_green.png");

    if (redLightLoaded && greenLightLoaded)
        cout << "Traffic light sprites loaded successfully!\n"
             << endl;

    // Create traffic signals
    vector<TrafficSignal> signals;
    signals.push_back(TrafficSignal(1, 1, cityMap.getNode(1).position, 8.0f, 6.0f, false));
    signals.push_back(TrafficSignal(2, 5, cityMap.getNode(5).position, 7.0f, 5.0f, false));
    signals.push_back(TrafficSignal(3, 6, cityMap.getNode(6).position, 6.0f, 6.0f, false));
    signals.push_back(TrafficSignal(4, 9, cityMap.getNode(9).position, 5.0f, 7.0f, false));

    cout << "Created " << signals.size() << " traffic signals\n"
         << endl;

    // CREATE ACTION STACK & EMERGENCY MANAGER
    ActionStack undoStack(50);
    EmergencyManager emergencyMgr;

    int nextVehicleID = 1;
    vector<int> vehiclesToRemove;

    // Spawn regular vehicles
    vector<Vehicle> cars;

    vector<int> path1 = cityMap.dijkstraAlgorithm(0, 11);
    Vehicle car1(nextVehicleID++, path1, 1.0, 0); // priority = 0 (regular)
    car1.setPosition(cityMap.getNode(0).position);
    car1.setStartPosition(cityMap.getNode(0).position);
    if (path1.size() > 1)
        car1.setTargetPosition(cityMap.getNode(path1[1]).position);
    cars.push_back(car1);

    vector<int> path2 = cityMap.dijkstraAlgorithm(2, 10);
    Vehicle car2(nextVehicleID++, path2, 0.8, 0);
    car2.setPosition(cityMap.getNode(2).position);
    car2.setStartPosition(cityMap.getNode(2).position);
    if (path2.size() > 1)
        car2.setTargetPosition(cityMap.getNode(path2[1]).position);
    cars.push_back(car2);

    vector<int> path3 = cityMap.dijkstraAlgorithm(0, 7);
    Vehicle car3(nextVehicleID++, path3, 1.2, 0);
    car3.setPosition(cityMap.getNode(0).position);
    car3.setStartPosition(cityMap.getNode(0).position);
    if (path3.size() > 1)
        car3.setTargetPosition(cityMap.getNode(path3[1]).position);
    cars.push_back(car3);

    cout << "Spawned " << cars.size() << " regular vehicles\n"
         << endl;

    // Print controls
    cout << "=== CONTROLS ===" << endl;
    cout << "1-4 - Toggle Signals" << endl;
    cout << "A   - Spawn Regular Vehicle" << endl;
    cout << "E   - Spawn Emergency Vehicle" << endl;
    cout << "U   - UNDO last action" << endl;
    cout << "ESC - Exit simulation" << endl;
    cout << "================\n"
         << endl;

    // Clock for delta time and timestamps
    sf::Clock clock;
    float totalTime = 0.0f;

    // Main loop
    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();
        totalTime += deltaTime;
        undoStack.setTime(totalTime);

        // Event handling
        while (const optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::KeyPressed>())
            {
                auto keyEvent = event->getIf<sf::Event::KeyPressed>();

                // Toggle signals (1-4)
                if (keyEvent->code == sf::Keyboard::Key::Num1)
                {
                    SignalState prevState = signals[0].getState();
                    signals[0].toggleLight();
                    SignalState newState = signals[0].getState();
                    undoStack.push(Action(ActionType::TOGGLE_SIGNAL, 1, prevState, newState, totalTime));
                    undoStack.printLastAction();
                }
                else if (keyEvent->code == sf::Keyboard::Key::Num2)
                {
                    SignalState prevState = signals[1].getState();
                    signals[1].toggleLight();
                    SignalState newState = signals[1].getState();
                    undoStack.push(Action(ActionType::TOGGLE_SIGNAL, 2, prevState, newState, totalTime));
                    undoStack.printLastAction();
                }
                else if (keyEvent->code == sf::Keyboard::Key::Num3)
                {
                    SignalState prevState = signals[2].getState();
                    signals[2].toggleLight();
                    SignalState newState = signals[2].getState();
                    undoStack.push(Action(ActionType::TOGGLE_SIGNAL, 3, prevState, newState, totalTime));
                    undoStack.printLastAction();
                }
                else if (keyEvent->code == sf::Keyboard::Key::Num4)
                {
                    SignalState prevState = signals[3].getState();
                    signals[3].toggleLight();
                    SignalState newState = signals[3].getState();
                    undoStack.push(Action(ActionType::TOGGLE_SIGNAL, 4, prevState, newState, totalTime));
                    undoStack.printLastAction();
                }
                // SPAWN REGULAR VEHICLE (A key)
                else if (keyEvent->code == sf::Keyboard::Key::A)
                {
                    // Random start and end nodes
                    int startNode = rand() % cityMap.getNumNodes();
                    while (hasSignal(signals, startNode))
                        startNode = rand() % cityMap.getNumNodes();

                    int endNode = rand() % cityMap.getNumNodes();
                    while (startNode == endNode)
                        endNode = rand() % cityMap.getNumNodes();
                    
                    vector<int> regularPath = cityMap.dijkstraAlgorithm(startNode, endNode);
                    
                    if (!regularPath.empty())
                    {                        
                        // Create regular vehicle (priority = 0)
                        Vehicle newCar(nextVehicleID++, regularPath, 1.0, 0);
                        newCar.setPosition(cityMap.getNode(startNode).position);
                        newCar.setStartPosition(cityMap.getNode(startNode).position);
                        if (regularPath.size() > 1)
                            newCar.setTargetPosition(cityMap.getNode(regularPath[1]).position);
                        
                        cars.push_back(newCar);
                        
                        // Record action for undo (store index in vector)
                        int vehicleIndex = cars.size() - 1;
                        undoStack.push(Action(ActionType::SPAWN_VEHICLE, newCar.getID(), vehicleIndex, totalTime));
                        
                        cout << "Regular Vehicle " << newCar.getID() << " spawned! Path: " << startNode << " -> " << endNode << endl;
                        undoStack.printLastAction();
                    }
                }
                // SPAWN EMERGENCY VEHICLE (E key)
                else if (keyEvent->code == sf::Keyboard::Key::E)
                {
                    // Random start and end nodes
                    int startNode = rand() % cityMap.getNumNodes();
                    while (hasSignal(signals, startNode))
                        startNode = rand() % cityMap.getNumNodes();

                    int endNode = rand() % cityMap.getNumNodes();
                    while (startNode == endNode)
                        endNode = rand() % cityMap.getNumNodes();

                    vector<int> emergencyPath = cityMap.dijkstraAlgorithm(startNode, endNode);

                    if (!emergencyPath.empty())
                    {
                        // Random priority between 5-10 (higher = more urgent)
                        int priority = 5 + (rand() % 6);

                        // Determine emergency type based on priority
                        string emergencyType = (priority >= 8) ? "Ambulance" : "Police";

                        Vehicle emergency(nextVehicleID++, emergencyPath, 1.5, priority);
                        emergency.setPosition(cityMap.getNode(startNode).position);
                        emergency.setStartPosition(cityMap.getNode(startNode).position);
                        if (emergencyPath.size() > 1)
                            emergency.setTargetPosition(cityMap.getNode(emergencyPath[1]).position);

                        cars.push_back(emergency);
                        emergencyMgr.addEmergency(&cars.back());

                        // Record action for undo (store index in vector)
                        int vehicleIndex = cars.size() - 1;
                        undoStack.push(Action(ActionType::SPAWN_VEHICLE, emergency.getID(), vehicleIndex, totalTime));

                        cout << emergencyType << " (Vehicle " << emergency.getID() << ") spawned! Priority: " << priority << ", Path: " << startNode << " -> " << endNode << endl;
                        undoStack.printLastAction();
                    }
                }
                // UNDO (U key)
                else if (keyEvent->code == sf::Keyboard::Key::U)
                {
                    if (undoStack.canUndo())
                    {
                        Action lastAction = undoStack.undo();

                        if (lastAction.type == ActionType::TOGGLE_SIGNAL)
                        {
                            for (auto &signal : signals)
                            {
                                if (signal.getID() == lastAction.targetID)
                                {
                                    signal.setState(lastAction.previousState);
                                    cout << "Signal " << lastAction.targetID << " reverted" << endl;
                                    break;
                                }
                            }
                        }
                        else if (lastAction.type == ActionType::SPAWN_VEHICLE)
                        {
                            bool found = false;
                            bool alreadyArrived = false;
                            for (auto& car : cars)
                            {
                                if (car.getID() == lastAction.targetID)
                                {
                                    if (car.hasArrivedDest())
                                    {
                                        alreadyArrived = true;
                                        cout << "Cannot undo Vehicle " << lastAction.targetID << " - already completed its journey!" << endl;
                                        break;
                                    }
                                    
                                    // Mark as arrived first (stops rendering/updating this frame)
                                    car.setStatus(VehicleStatus::ARRIVED);
                                    
                                    // Schedule for removal at end of frame
                                    vehiclesToRemove.push_back(car.getID());
                                    found = true;
                                    
                                    cout << "Vehicle " << lastAction.targetID << " spawn undone (removed from simulation)" << endl;
                                    break;
                                }
                            }
                            
                            if (!found)
                            {
                                cout << "Warning: Could not find vehicle " << lastAction.targetID << " to undo!" << endl;
                            }
                        }
                    }
                    else
                    {
                        cout << "Nothing to undo!" << endl;
                    }
                }
                else if (keyEvent->code == sf::Keyboard::Key::Escape)
                {
                    window.close();
                }
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

                if (car.getInterpolation() >= 1.0f)
                {
                    int currentNodeId = car.getCurrentNode();
                    int nextNodeId = car.getNextNode();

                    if (nextNodeId != -1)
                    {
                        bool blockedBySignal = false;

                        // Emergency vehicles BYPASS red signals!
                        if (!car.isEmergency())
                        {
                            for (auto &signal : signals)
                            {
                                if (signal.getIntersectionID() == currentNodeId && signal.getState() == SignalState::RED)
                                {
                                    signal.addVehicleToQueue(&car);
                                    blockedBySignal = true;
                                    break;
                                }
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
                        cout << "Vehicle " << car.getID() << " arrived!" << endl;
                    }
                }
            }
        }

        if (!vehiclesToRemove.empty())
        {
            for (int vehicleID : vehiclesToRemove)
            {
                // Find and erase vehicle
                auto it = remove_if(cars.begin(), cars.end(), [vehicleID](const Vehicle& v) { return v.getID() == vehicleID; });
                
                if (it != cars.end())
                {
                    cars.erase(it, cars.end());
                    cout << "Vehicle " << vehicleID << " physically removed from vector" << endl;
                }
            }
            vehiclesToRemove.clear();
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

        // --- DRAW TRAFFIC SIGNALS ---
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

        // --- DRAW VEHICLES ---
        for (int i = 0; i < cars.size(); i++)
        {
            const auto &car = cars[i];
            if (!car.hasArrivedDest())
            {
                sf::Vector2<float> carPos = car.getPosition();

                // Determine which texture to use
                bool drewSprite = false;

                if (car.isEmergency())
                {
                    // Choose sprite based on priority
                    // High priority (8-10) = Ambulance (red pulsing)
                    // Lower priority (5-7) = Police (blue pulsing)
                    bool useAmbulance = (car.getPriority() >= 8);

                    if ((useAmbulance && ambulanceLoaded) || (!useAmbulance && policeLoaded))
                    {
                        sf::Sprite emergencySprite(useAmbulance ? ambulanceTexture : policeTexture);
                        sf::FloatRect bounds = emergencySprite.getLocalBounds();
                        emergencySprite.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
                        emergencySprite.setPosition(carPos);
                        emergencySprite.setScale({0.35f, 0.35f});

                        // Pulsing effect: Red for ambulance, Blue for police
                        float pulse = sin(totalTime * 8.0f) * 0.3f + 0.7f;

                        if (useAmbulance)
                        {
                            // Red pulsing for ambulance
                            emergencySprite.setColor(sf::Color(255, static_cast<int>(100 * pulse), static_cast<int>(100 * pulse)));
                        }
                        else
                        {
                            // Blue pulsing for police
                            emergencySprite.setColor(sf::Color(static_cast<int>(150 * pulse), static_cast<int>(150 * pulse), 255));
                        }

                        window.draw(emergencySprite);
                        drewSprite = true;
                    }
                }
                else
                {
                    // Draw regular vehicle
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
                        drewSprite = true;
                    }
                }

                // Fallback rectangles
                if (!drewSprite)
                {
                    sf::RectangleShape carRect({20, 12});
                    carRect.setPosition(carPos - sf::Vector2<float>(10, 6));

                    if (car.isEmergency())
                    {
                        // Red for ambulance (high priority), Blue for police (lower priority)
                        if (car.getPriority() >= 8)
                            carRect.setFillColor(sf::Color(255, 0, 0, 200)); // Bright red
                        else
                            carRect.setFillColor(sf::Color(0, 0, 255, 200)); // Bright blue
                    }
                    else if (car.getStatus() == VehicleStatus::WAITING)
                        carRect.setFillColor(sf::Color::Yellow);
                    else
                        carRect.setFillColor(sf::Color::White);

                    window.draw(carRect);
                }
            }
        }

        // --- DRAW HUD ---
        if (fontLoaded)
        {
            int moving = 0, waiting = 0, arrived = 0, emergency = 0;
            for (const auto &car : cars)
            {
                if (car.hasArrivedDest())
                    arrived++;
                else if (car.isEmergency())
                {
                    emergency++;
                    moving++;
                }
                else if (car.getStatus() == VehicleStatus::WAITING)
                    waiting++;
                else
                    moving++;
            }

            sf::Text hud(font);
            string hudText = "Moving: " + to_string(moving) + " | Waiting: " + to_string(waiting) + " | Emergency: " + to_string(emergency) + " | Arrived: " + to_string(arrived) + " | Undo: " + to_string(undoStack.size());
            hud.setString(hudText);
            hud.setCharacterSize(16);
            hud.setFillColor(sf::Color::White);
            hud.setPosition({10, 10});
            window.draw(hud);

            sf::Text controls(font);
            controls.setString("1-4: Toggle Signals | A: Spawn Regular Car | E: Spawn Emergency Car | U: Undo | ESC: Exit");
            controls.setCharacterSize(14);
            controls.setFillColor(sf::Color(180, 180, 180));
            controls.setPosition({10, 570});
            window.draw(controls);
        }

        window.display();
    }

    cout << "\n=== Simulation Ended ===" << endl;
    emergencyMgr.printQueue();

    return 0;
}