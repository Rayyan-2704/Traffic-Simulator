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

Simulation::Simulation() : window(sf::VideoMode({1200, 800}), "Traffic Simulator"), undoStack(100)
{
    nextVehicleID = 1;
    totalTime = 0.0f;
    isRunning = false;
    srand(time(nullptr));

    cout << "\n=== Traffic Simulator - Initialized ===\n" << endl;
}

void Simulation::loadAssets()
{
    cout << "Loading assets..." << endl;

    // Load font (only once)
    if (!fontLoaded)
    {
        fontLoaded = font.openFromFile("assets/font.ttf");
        if (fontLoaded)
            cout << "Font loaded successfully" << endl;
    }

    // Load car textures (only once)
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

    // Load emergency vehicle textures (only once)
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

    // Load traffic light sprites (only once)
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

    cout << "Assets loaded!\n"
         << endl;
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
    // 4 traffic signals: 2 manual, 2 automatic
    signals.push_back(TrafficSignal(1, 1, cityMap.getNode(1).position, 8.0f, 6.0f, false)); // Manual
    signals.push_back(TrafficSignal(2, 5, cityMap.getNode(5).position, 7.0f, 5.0f, true));  // Auto
    signals.push_back(TrafficSignal(3, 6, cityMap.getNode(6).position, 6.0f, 6.0f, false)); // Manual
    signals.push_back(TrafficSignal(4, 9, cityMap.getNode(9).position, 5.0f, 7.0f, true));  // Auto

    cout << "Created " << signals.size() << " traffic signals (2 manual, 2 auto)\n" << endl;
}

void Simulation::spawnInitialVehicles()
{
    // Spawn 3 initial regular vehicles
    vector<int> path1 = cityMap.dijkstraAlgorithm(0, 11);
    Vehicle car1(nextVehicleID++, path1, 1.0, 0);
    car1.setPosition(cityMap.getNode(0).position);
    car1.setStartPosition(cityMap.getNode(0).position);
    if (path1.size() > 1)
        car1.setTargetPosition(cityMap.getNode(path1[1]).position);
    vehicles.push_back(car1);
    analytics.recordVehicleSpawn(car1.getID(), 0.0f, false);

    vector<int> path2 = cityMap.dijkstraAlgorithm(2, 10);
    Vehicle car2(nextVehicleID++, path2, 0.8, 0);
    car2.setPosition(cityMap.getNode(2).position);
    car2.setStartPosition(cityMap.getNode(2).position);
    if (path2.size() > 1)
        car2.setTargetPosition(cityMap.getNode(path2[1]).position);
    vehicles.push_back(car2);
    analytics.recordVehicleSpawn(car2.getID(), 0.0f, false);

    vector<int> path3 = cityMap.dijkstraAlgorithm(0, 7);
    Vehicle car3(nextVehicleID++, path3, 1.2, 0);
    car3.setPosition(cityMap.getNode(0).position);
    car3.setStartPosition(cityMap.getNode(0).position);
    if (path3.size() > 1)
        car3.setTargetPosition(cityMap.getNode(path3[1]).position);
    vehicles.push_back(car3);
    analytics.recordVehicleSpawn(car3.getID(), 0.0f, false);

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
    // Load all assets
    loadAssets();
    
    // Initialize simulation components
    initializeSignals();
    spawnInitialVehicles();
    
    // Print controls
    cout << "=== CONTROLS ===" << endl;
    cout << "1-4 - Toggle Signals 1-4" << endl;
    cout << "A   - Spawn Regular Vehicle" << endl;
    cout << "E   - Spawn Emergency Vehicle" << endl;
    cout << "U   - UNDO last action" << endl;
    cout << "ESC - Exit simulation" << endl;
    cout << "================\n" << endl;
    
    isRunning = true;
    
    // Main game loop
    while (window.isOpen() && isRunning)
    {
        float deltaTime = clock.restart().asSeconds();
        totalTime += deltaTime;
        undoStack.setTime(totalTime);
        
        handleInput();
        update(deltaTime);
        render();
    }
    
    emergencyMgr.printQueue();
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
            
            // Toggle signals
            if (keyEvent->code == sf::Keyboard::Key::Num1)
                toggleSignal(0);
            else if (keyEvent->code == sf::Keyboard::Key::Num2)
                toggleSignal(1);
            else if (keyEvent->code == sf::Keyboard::Key::Num3)
                toggleSignal(2);
            else if (keyEvent->code == sf::Keyboard::Key::Num4)
                toggleSignal(3);
            // Spawn vehicles
            else if (keyEvent->code == sf::Keyboard::Key::A)
                spawnRegularVehicle();
            else if (keyEvent->code == sf::Keyboard::Key::E)
                spawnEmergencyVehicle();
            // Undo
            else if (keyEvent->code == sf::Keyboard::Key::U)
                undoLastAction();
            // Exit
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
    if (signalIndex < 0 || signalIndex >= signals.size()) return;
    
    SignalState prevState = signals[signalIndex].getState();
    signals[signalIndex].toggleLight();
    SignalState newState = signals[signalIndex].getState();
    
    undoStack.push(Action(ActionType::TOGGLE_SIGNAL, signals[signalIndex].getID(), prevState, newState, totalTime));
    undoStack.printLastAction();

    // Track analytics
    analytics.recordSignalToggle();
}

void Simulation::spawnRegularVehicle()
{
    // Random start node (not at signal)
    int startNode = rand() % cityMap.getNumNodes();
    while (hasSignal(startNode))
        startNode = rand() % cityMap.getNumNodes();
    
    // Random end node (different from start)
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
        
        int vehicleIndex = vehicles.size() - 1;
        undoStack.push(Action(ActionType::SPAWN_VEHICLE, newCar.getID(), vehicleIndex, totalTime));

        // Track analytics
        analytics.recordVehicleSpawn(newCar.getID(), totalTime, false);
        
        cout << "Regular Vehicle " << newCar.getID() << " spawned! Path: " << startNode << " -> " << endNode << endl;
        undoStack.printLastAction();
    }
}

void Simulation::spawnEmergencyVehicle()
{
    // Random start node (not at signal)
    int startNode = rand() % cityMap.getNumNodes();
    while (hasSignal(startNode))
        startNode = rand() % cityMap.getNumNodes();
    
    // Random end node (different from start)
    int endNode = rand() % cityMap.getNumNodes();
    while (startNode == endNode)
        endNode = rand() % cityMap.getNumNodes();
    
    vector<int> path = cityMap.dijkstraAlgorithm(startNode, endNode);
    
    if (!path.empty())
    {
        int priority = 5 + (rand() % 6); // Priority 5-10
        string emergencyType = (priority >= 8) ? "Ambulance" : "Police";
        
        Vehicle emergency(nextVehicleID++, path, 1.5, priority);
        emergency.setPosition(cityMap.getNode(startNode).position);
        emergency.setStartPosition(cityMap.getNode(startNode).position);
        if (path.size() > 1)
            emergency.setTargetPosition(cityMap.getNode(path[1]).position);
        
        vehicles.push_back(emergency);
        emergencyMgr.addEmergency(&vehicles.back());
        
        int vehicleIndex = vehicles.size() - 1;
        undoStack.push(Action(ActionType::SPAWN_VEHICLE, emergency.getID(), vehicleIndex, totalTime));

        // Track analytics
        analytics.recordVehicleSpawn(emergency.getID(), totalTime, true);
        
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

    // Track analytics
    analytics.recordUndoOperation();
    
    if (lastAction.type == ActionType::TOGGLE_SIGNAL)
    {
        // Undo signal toggle
        for (auto& signal : signals)
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
        // Undo vehicle spawn
        bool found = false;
        
        for (auto& car : vehicles)
        {
            if (car.getID() == lastAction.targetID)
            {
                found = true;
                
                // Check if vehicle already arrived naturally
                if (car.hasArrivedDest())
                {
                    cout << "Cannot undo Vehicle " << lastAction.targetID << " - already completed its journey!" << endl;
                    break;
                }
                
                // Mark for removal
                car.setStatus(VehicleStatus::ARRIVED);
                vehiclesToRemove.push_back(car.getID());
                
                cout << "Vehicle " << lastAction.targetID << " spawn undone (removed from simulation)" << endl;
                break;
            }
        }
        
        if (!found)
            cout << "Warning: Could not find vehicle " << lastAction.targetID << " to undo!" << endl;
    }
}

void Simulation::update(float deltaTime)
{
    // Update automatic signals
    for (auto& signal : signals)
    {
        if (signal.isAutoMode())
            signal.updateTimer(deltaTime);
    }
    
    // Update vehicles
    for (auto& car : vehicles)
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
                    
                    // Emergency vehicles bypass red signals
                    if (!car.isEmergency())
                    {
                        for (auto& signal : signals)
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

                        // Track road usage for analytics
                        analytics.recordRoadUsage(currentNodeId, nextNodeId);
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
    
    // Remove undone vehicles (deferred removal for safety)
    if (!vehiclesToRemove.empty())
    {
        for (int vehicleID : vehiclesToRemove)
        {
            auto it = remove_if(vehicles.begin(), vehicles.end(), [vehicleID](const Vehicle& v) { return v.getID() == vehicleID; });
            
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
    for (const auto& nodePair : cityMap.getAllNodes())
    {
        int nodeId = nodePair.first;
        const Node& node = nodePair.second;
        
        for (const Edge& edge : cityMap.getNeighbours(nodeId))
        {
            const Node& neighbor = cityMap.getNode(edge.destination);
            sf::Vertex line[] = {
                sf::Vertex{node.position, sf::Color(100, 100, 100)},
                sf::Vertex{neighbor.position, sf::Color(100, 100, 100)}
            };
            window.draw(line, 2, sf::PrimitiveType::Lines);
        }
    }
}

void Simulation::drawIntersections()
{
    for (const auto& nodePair : cityMap.getAllNodes())
    {
        const Node& node = nodePair.second;
        
        sf::CircleShape intersection(16);
        intersection.setPosition(node.position - sf::Vector2<float>(16, 16));
        intersection.setFillColor(sf::Color(200, 200, 200));
        intersection.setOutlineThickness(3);
        intersection.setOutlineColor(sf::Color::White);
        window.draw(intersection);
        
        if (fontLoaded)
        {
            sf::Text label(font);
            label.setString(to_string(node.ID));
            label.setCharacterSize(20);
            label.setFillColor(sf::Color::Black);
            label.setPosition(node.position - sf::Vector2<float>(7, 10));
            window.draw(label);
        }
    }
}

void Simulation::drawTrafficSignals()
{
    for (const auto& signal : signals)
    {
        sf::Vector2<float> sigPos = signal.getPosition();
        bool usedSprite = false;
        
        // Draw appropriate sprite based on state
        if (signal.getState() == SignalState::RED && redLightLoaded)
        {
            sf::Sprite lightSprite(trafficLightRed);
            lightSprite.setScale({0.5f, 0.5f});
            lightSprite.setPosition(sigPos + sf::Vector2<float>(20, -40));
            window.draw(lightSprite);
            usedSprite = true;
        }
        else if (signal.getState() == SignalState::GREEN && greenLightLoaded)
        {
            sf::Sprite lightSprite(trafficLightGreen);
            lightSprite.setScale({0.5f, 0.5f});
            lightSprite.setPosition(sigPos + sf::Vector2<float>(20, -40));
            window.draw(lightSprite);
            usedSprite = true;
        }
        
        // Fallback to colored circle
        if (!usedSprite)
        {
            sf::CircleShape signalCircle(12);
            signalCircle.setPosition(sigPos + sf::Vector2<float>(25, -25));
            signalCircle.setFillColor(signal.getColor());
            signalCircle.setOutlineThickness(3);
            signalCircle.setOutlineColor(sf::Color::Black);
            window.draw(signalCircle);
        }
        
        // Draw labels
        if (fontLoaded)
        {
            sf::Text sigLabel(font);
            sigLabel.setString("S" + to_string(signal.getID()));
            sigLabel.setCharacterSize(16);
            sigLabel.setFillColor(sf::Color::White);
            sigLabel.setOutlineColor(sf::Color::Black);
            sigLabel.setOutlineThickness(2);
            sigLabel.setPosition(sigPos + sf::Vector2<float>(30, -5));
            window.draw(sigLabel);
            
            // Draw queue size
            if (signal.getQueueSize() > 0)
            {
                sf::Text queueLabel(font);
                queueLabel.setString("Q:" + to_string(signal.getQueueSize()));
                queueLabel.setCharacterSize(18);
                queueLabel.setFillColor(sf::Color::Yellow);
                queueLabel.setOutlineColor(sf::Color::Black);
                queueLabel.setOutlineThickness(2);
                queueLabel.setPosition(sigPos + sf::Vector2<float>(25, 12));
                window.draw(queueLabel);
            }
        }
    }
}

void Simulation::drawVehicles()
{
    for (int i = 0; i < vehicles.size(); i++)
    {
        const auto& car = vehicles[i];
        if (car.hasArrivedDest())
            continue;
        
        sf::Vector2<float> carPos = car.getPosition();
        bool drewSprite = false;
        
        if (car.isEmergency())
        {
            // Emergency vehicle rendering
            bool useAmbulance = (car.getPriority() >= 8);
            
            if ((useAmbulance && ambulanceLoaded) || (!useAmbulance && policeLoaded))
            {
                sf::Sprite emergencySprite(useAmbulance ? ambulanceTexture : policeTexture);
                sf::FloatRect bounds = emergencySprite.getLocalBounds();
                emergencySprite.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
                emergencySprite.setPosition(carPos);
                emergencySprite.setScale({0.35f, 0.35f});
                
                // Pulsing effect
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
            // Regular vehicle rendering
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
    
    // Count vehicles by status
    int moving = 0, waiting = 0, arrived = 0, emergency = 0;
    for (const auto& car : vehicles)
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
    
    // Main HUD
    sf::Text hud(font);
    string hudText = "Moving: " + to_string(moving) + " | Waiting: " + to_string(waiting) + " | Emergency: " + to_string(emergency) + " | Arrived: " + to_string(arrived) + " | Undo: " + to_string(undoStack.size());
    hud.setString(hudText);
    hud.setCharacterSize(22);
    hud.setFillColor(sf::Color::White);
    hud.setOutlineColor(sf::Color::Black);
    hud.setOutlineThickness(2);
    hud.setPosition({15, 15});
    window.draw(hud);
    
    // Controls
    sf::Text controls(font);
    controls.setString("1-4: Toggle Signals | A: Spawn Car | E: Emergency | U: Undo | ESC: Exit");
    controls.setCharacterSize(18);
    controls.setFillColor(sf::Color(200, 200, 200));
    controls.setOutlineColor(sf::Color::Black);
    controls.setOutlineThickness(1);
    controls.setPosition({15, 760});
    window.draw(controls);
}

void Simulation::saveAnalytics(const std::string& filename) { analytics.saveToFile(filename); }
void Simulation::printAnalytics() const { analytics.printSummary(); }

Simulation::~Simulation()
{
    cout << "\n=== Simulation Ended ===" << endl;

    // Print summary & Save to file
    analytics.updateStats(vehicles, signals, totalTime); 
    analytics.printSummary();
    saveAnalytics("data/simulation_log.txt");
}