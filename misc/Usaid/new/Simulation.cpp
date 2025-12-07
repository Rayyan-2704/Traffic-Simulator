#include "../include/Simulation.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>

sf::Font Simulation::font;
bool Simulation::fontLoaded = false;

vector<sf::Texture> Simulation::carTextures(5);
vector<bool> Simulation::carTextureLoaded(5, false);

sf::Texture Simulation::ambulanceTexture;
sf::Texture Simulation::policeTexture;
bool Simulation::ambulanceLoaded = false;
bool Simulation::policeLoaded = false;

sf::Texture Simulation::trafficLightRed;
sf::Texture Simulation::trafficLightGreen;
bool Simulation::redLightLoaded = false;
bool Simulation::greenLightLoaded = false;

Simulation::Simulation() : window(sf::VideoMode({800, 600}), "Traffic Simulator - Analytics & Logging"), 
                           undoStack(100), logger("data/simulation_log.txt")
{
    nextVehicleID = 1;
    totalTime = 0.0f;
    isRunning = false;
    srand(time(nullptr));

    cout << "\n=== Traffic Simulator - Day 13: Analytics & Logging ===\n" << endl;
}

void Simulation::loadAssets()
{
    cout << "Loading assets..." << endl;

    if (!fontLoaded)
    {
        fontLoaded = font.openFromFile("assets/font.ttf");
        if (fontLoaded)
            cout << "Font loaded successfully" << endl;
    }

    vector<string> carFiles = {
        "assets/images/vehicles/car_blue.png",
        "assets/images/vehicles/car_red.png",
        "assets/images/vehicles/car_green.png",
        "assets/images/vehicles/car_yellow.png",
        "assets/images/vehicles/car_black.png"};

    for (int i = 0; i < 5; i++)
    {
        if (!carTextureLoaded[i])
        {
            carTextureLoaded[i] = carTextures[i].loadFromFile(carFiles[i]);
            if (carTextureLoaded[i])
                cout << "Loaded: " << carFiles[i] << endl;
        }
    }

    if (!ambulanceLoaded)
    {
        ambulanceLoaded = ambulanceTexture.loadFromFile("assets/images/vehicles/ambulance.png");
        if (ambulanceLoaded)
            cout << "Ambulance sprite loaded" << endl;
    }

    if (!policeLoaded)
    {
        policeLoaded = policeTexture.loadFromFile("assets/images/vehicles/police.png");
        if (policeLoaded)
            cout << "Police sprite loaded" << endl;
    }

    if (!redLightLoaded)
    {
        redLightLoaded = trafficLightRed.loadFromFile("assets/images/signals/trafficLight_red.png");
        if (redLightLoaded)
            cout << "Red traffic light loaded" << endl;
    }

    if (!greenLightLoaded)
    {
        greenLightLoaded = trafficLightGreen.loadFromFile("assets/images/signals/trafficLight_green.png");
        if (greenLightLoaded)
            cout << "Green traffic light loaded" << endl;
    }

    cout << "Assets loaded!\n" << endl;
}

bool Simulation::loadMap(const string &filename)
{
    if (!cityMap.loadFromFile(filename))
    {
        cout << "Failed to load map: " << filename << endl;
        return false;
    }

    cout << "City loaded: " << cityMap.getNumNodes() << " nodes\n" << endl;
    return true;
}

void Simulation::initializeSignals()
{
    signals.push_back(TrafficSignal(1, 1, cityMap.getNode(1).position, 8.0f, 6.0f, false));
    signals.push_back(TrafficSignal(2, 5, cityMap.getNode(5).position, 7.0f, 5.0f, true));
    signals.push_back(TrafficSignal(3, 6, cityMap.getNode(6).position, 6.0f, 6.0f, false));
    signals.push_back(TrafficSignal(4, 9, cityMap.getNode(9).position, 5.0f, 7.0f, true));

    cout << "Created " << signals.size() << " traffic signals (2 manual, 2 auto)\n" << endl;
}

void Simulation::spawnInitialVehicles()
{
    vector<int> path1 = cityMap.dijkstraAlgorithm(0, 11);
    Vehicle car1(nextVehicleID++, path1, 1.0, 0);
    car1.setPosition(cityMap.getNode(0).position);
    car1.setStartPosition(cityMap.getNode(0).position);
    if (path1.size() > 1)
        car1.setTargetPosition(cityMap.getNode(path1[1]).position);
    vehicles.push_back(car1);
    analytics.recordVehicleSpawn(car1.getID(), totalTime, false);

    vector<int> path2 = cityMap.dijkstraAlgorithm(2, 10);
    Vehicle car2(nextVehicleID++, path2, 0.8, 0);
    car2.setPosition(cityMap.getNode(2).position);
    car2.setStartPosition(cityMap.getNode(2).position);
    if (path2.size() > 1)
        car2.setTargetPosition(cityMap.getNode(path2[1]).position);
    vehicles.push_back(car2);
    analytics.recordVehicleSpawn(car2.getID(), totalTime, false);

    vector<int> path3 = cityMap.dijkstraAlgorithm(0, 7);
    Vehicle car3(nextVehicleID++, path3, 1.2, 0);
    car3.setPosition(cityMap.getNode(0).position);
    car3.setStartPosition(cityMap.getNode(0).position);
    if (path3.size() > 1)
        car3.setTargetPosition(cityMap.getNode(path3[1]).position);
    vehicles.push_back(car3);
    analytics.recordVehicleSpawn(car3.getID(), totalTime, false);

    cout << "Spawned " << vehicles.size() << " initial vehicles\n" << endl;
}

bool Simulation::hasSignal(int nodeID) const
{
    for (const auto &signal : signals)
    {
        if (signal.getIntersectionID() == nodeID)
            return true;
    }
    return false;
}

void Simulation::setFrameRate(int fps)
{
    window.setFramerateLimit(fps);
}

void Simulation::run()
{
    loadAssets();
    initializeSignals();
    spawnInitialVehicles();

    analytics.setSimulationStartTime(0.0f);

    cout << "=== CONTROLS ===" << endl;
    cout << "1-4 - Toggle Signals 1-4" << endl;
    cout << "A   - Spawn Regular Vehicle" << endl;
    cout << "E   - Spawn Emergency Vehicle" << endl;
    cout << "U   - UNDO last action" << endl;
    cout << "S   - Show Statistics" << endl;
    cout << "ESC - Exit simulation" << endl;
    cout << "================\n" << endl;

    isRunning = true;

    while (window.isOpen() && isRunning)
    {
        float deltaTime = clock.restart().asSeconds();
        totalTime += deltaTime;
        undoStack.setTime(totalTime);

        handleInput();
        update(deltaTime);
        render();
    }

    analytics.setSimulationEndTime(totalTime);
    printFinalStatistics();
    saveSimulationLog();
}

void Simulation::handleInput()
{
    while (const optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
            isRunning = false;
        }

        if (event->is<sf::Event::KeyPressed>())
        {
            auto keyEvent = event->getIf<sf::Event::KeyPressed>();

            if (keyEvent->code == sf::Keyboard::Key::Num1)
                toggleSignal(0);
            else if (keyEvent->code == sf::Keyboard::Key::Num2)
                toggleSignal(1);
            else if (keyEvent->code == sf::Keyboard::Key::Num3)
                toggleSignal(2);
            else if (keyEvent->code == sf::Keyboard::Key::Num4)
                toggleSignal(3);
            else if (keyEvent->code == sf::Keyboard::Key::A)
                spawnRegularVehicle();
            else if (keyEvent->code == sf::Keyboard::Key::E)
                spawnEmergencyVehicle();
            else if (keyEvent->code == sf::Keyboard::Key::U)
                undoLastAction();
            else if (keyEvent->code == sf::Keyboard::Key::S)
            {
                analytics.printStatistics();
                analytics.printBusiestRoads(5);
                analytics.printBusiestIntersections(signals, 3);
            }
            else if (keyEvent->code == sf::Keyboard::Key::Escape)
            {
                window.close();
                isRunning = false;
            }
        }
    }
}

void Simulation::toggleSignal(int signalIndex)
{
    if (signalIndex < 0 || signalIndex >= signals.size())
        return;

    SignalState prevState = signals[signalIndex].getState();
    signals[signalIndex].toggleLight();
    SignalState newState = signals[signalIndex].getState();

    undoStack.push(Action(ActionType::TOGGLE_SIGNAL, signals[signalIndex].getID(), prevState, newState, totalTime));
    analytics.recordSignalToggle();
    undoStack.printLastAction();
}

void Simulation::spawnRegularVehicle()
{
    int startNode = rand() % cityMap.getNumNodes();
    while (hasSignal(startNode))
        startNode = rand() % cityMap.getNumNodes();

    int endNode = rand() % cityMap.getNumNodes();
    while (startNode == endNode)
        endNode = rand() % cityMap.getNumNodes();

    vector<int> path = cityMap.dijkstraAlgorithm(startNode, endNode);

    if (!path.empty())
    {
        Vehicle newCar(nextVehicleID++, path, 1.0, 0);
        newCar.setPosition(cityMap.getNode(startNode).position);
        newCar.setStartPosition(cityMap.getNode(startNode).position);
        if (path.size() > 1)
            newCar.setTargetPosition(cityMap.getNode(path[1]).position);

        vehicles.push_back(newCar);
        analytics.recordVehicleSpawn(newCar.getID(), totalTime, false);

        int vehicleIndex = vehicles.size() - 1;
        undoStack.push(Action(ActionType::SPAWN_VEHICLE, newCar.getID(), vehicleIndex, totalTime));

        cout << "Regular Vehicle " << newCar.getID() << " spawned! Path: " << startNode << " -> " << endNode << endl;
        undoStack.printLastAction();
    }
}

void Simulation::spawnEmergencyVehicle()
{
    int startNode = rand() % cityMap.getNumNodes();
    while (hasSignal(startNode))
        startNode = rand() % cityMap.getNumNodes();

    int endNode = rand() % cityMap.getNumNodes();
    while (startNode == endNode)
        endNode = rand() % cityMap.getNumNodes();

    vector<int> path = cityMap.dijkstraAlgorithm(startNode, endNode);

    if (!path.empty())
    {
        int priority = 5 + (rand() % 6);
        string emergencyType = (priority >= 8) ? "Ambulance" : "Police";

        Vehicle emergency(nextVehicleID++, path, 1.5, priority);
        emergency.setPosition(cityMap.getNode(startNode).position);
        emergency.setStartPosition(cityMap.getNode(startNode).position);
        if (path.size() > 1)
            emergency.setTargetPosition(cityMap.getNode(path[1]).position);

        vehicles.push_back(emergency);
        emergencyMgr.addEmergency(&vehicles.back());
        analytics.recordVehicleSpawn(emergency.getID(), totalTime, true);

        int vehicleIndex = vehicles.size() - 1;
        undoStack.push(Action(ActionType::SPAWN_VEHICLE, emergency.getID(), vehicleIndex, totalTime));

        cout << emergencyType << " (Vehicle " << emergency.getID() << ") spawned! Priority: " << priority << ", Path: " << startNode << " -> " << endNode << endl;
        undoStack.printLastAction();
    }
}

void Simulation::undoLastAction()
{
    if (!undoStack.canUndo())
    {
        cout << "Nothing to undo!" << endl;
        return;
    }

    Action lastAction = undoStack.undo();
    analytics.recordUndo();

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
        for (auto &car : vehicles)
        {
            if (car.getID() == lastAction.targetID)
            {
                if (car.hasArrivedDest())
                {
                    cout << "Cannot undo Vehicle " << lastAction.targetID << " - already completed its journey!" << endl;
                    break;
                }

                car.setStatus(VehicleStatus::ARRIVED);
                vehiclesToRemove.push_back(car.getID());

                cout << "Vehicle " << lastAction.targetID << " spawn undone (removed from simulation)" << endl;
                break;
            }
        }
    }
}

void Simulation::trackVehicleMovement(Vehicle &car)
{
    int currentNode = car.getCurrentNode();
    int nextNode = car.getNextNode();

    if (nextNode != -1)
    {
        analytics.recordRoadUsage(currentNode, nextNode);
        analytics.recordIntersectionPass(currentNode);
    }
}

void Simulation::update(float deltaTime)
{
    for (auto &signal : signals)
    {
        if (signal.isAutoMode())
            signal.updateTimer(deltaTime);
    }

    for (auto &car : vehicles)
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
                        trackVehicleMovement(car);
                        sf::Vector2<float> nextPos = cityMap.getNode(nextNodeId).position;
                        car.moveToNextNode(nextPos);
                    }
                }
                else
                {
                    car.setStatus(VehicleStatus::ARRIVED);
                    analytics.recordVehicleArrival(car.getID(), totalTime);
                    cout << "Vehicle " << car.getID() << " arrived at destination!" << endl;
                }
            }
        }
    }

    if (!vehiclesToRemove.empty())
    {
        for (int vehicleID : vehiclesToRemove)
        {
            auto it = remove_if(vehicles.begin(), vehicles.end(),
                                [vehicleID](const Vehicle &v)
                                { return v.getID() == vehicleID; });

            if (it != vehicles.end())
            {
                vehicles.erase(it, vehicles.end());
                cout << "Vehicle " << vehicleID << " physically removed from simulation" << endl;
            }
        }
        vehiclesToRemove.clear();
    }
}

void Simulation::render()
{
    window.clear(sf::Color(30, 30, 30));

    drawRoads();
    drawIntersections();
    drawTrafficSignals();
    drawVehicles();
    drawHUD();

    window.display();
}

void Simulation::drawRoads()
{
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
}

void Simulation::drawIntersections()
{
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
}

void Simulation::drawTrafficSignals()
{
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
}

void Simulation::drawVehicles()
{
    for (int i = 0; i < vehicles.size(); i++)
    {
        const auto &car = vehicles[i];
        if (car.hasArrivedDest())
            continue;

        sf::Vector2<float> carPos = car.getPosition();
        bool drewSprite = false;

        if (car.isEmergency())
        {
            bool useAmbulance = (car.getPriority() >= 8);

            if ((useAmbulance && ambulanceLoaded) || (!useAmbulance && policeLoaded))
            {
                sf::Sprite emergencySprite(useAmbulance ? ambulanceTexture : policeTexture);
                sf::FloatRect bounds = emergencySprite.getLocalBounds();
                emergencySprite.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
                emergencySprite.setPosition(carPos);
                emergencySprite.setScale({0.35f, 0.35f});

                float pulse = sin(totalTime * 8.0f) * 0.3f + 0.7f;

                if (useAmbulance)
                    emergencySprite.setColor(sf::Color(255, static_cast<int>(100 * pulse), static_cast<int>(100 * pulse)));
                else
                    emergencySprite.setColor(sf::Color(static_cast<int>(150 * pulse), static_cast<int>(150 * pulse), 255));

                window.draw(emergencySprite);
                drewSprite = true;
            }
        }
        else
        {
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

        if (!drewSprite)
        {
            sf::RectangleShape carRect({20, 12});
            carRect.setPosition(carPos - sf::Vector2<float>(10, 6));

            if (car.isEmergency())
            {
                if (car.getPriority() >= 8)
                    carRect.setFillColor(sf::Color(255, 0, 0, 200));
                else
                    carRect.setFillColor(sf::Color(0, 0, 255, 200));
            }
            else if (car.getStatus() == VehicleStatus::WAITING)
                carRect.setFillColor(sf::Color::Yellow);
            else
                carRect.setFillColor(sf::Color::White);

            window.draw(carRect);
        }
    }
}

void Simulation::drawHUD()
{
    if (!fontLoaded)
        return;

    int moving = 0, waiting = 0, arrived = 0, emergency = 0;
    for (const auto &car : vehicles)
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
    controls.setString("1-4: Signals | A: Car | E: Emergency | U: Undo | S: Stats | ESC: Exit");
    controls.setCharacterSize(14);
    controls.setFillColor(sf::Color(180, 180, 180));
    controls.setPosition({10, 570});
    window.draw(controls);
}

void Simulation::printFinalStatistics()
{
    cout << "\n\n";
    analytics.printStatistics();
    analytics.printBusiestRoads(5);
    analytics.printBusiestIntersections(signals, 3);
}

void Simulation::saveSimulationLog()
{
    logger.generateCompleteLog(analytics, signals);
}

Simulation::~Simulation()
{
    cout << "\n=== Simulation Ended ===" << endl;
}
