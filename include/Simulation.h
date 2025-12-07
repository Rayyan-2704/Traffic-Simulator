#ifndef SIMULATION_H
#define SIMULATION_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include "Graph.h"
#include "Vehicle.h"
#include "TrafficSignal.h"
#include "ActionStack.h"
#include "EmergencyManager.h"
#include "Analytics.h"
using namespace std;

class Simulation
{
    // Core components
    Graph cityMap;
    vector<Vehicle> vehicles;
    vector<TrafficSignal> signals;
    ActionStack undoStack;
    EmergencyManager emergencyMgr;
    Analytics analytics;

    // SFML Window
    sf::RenderWindow window;
    sf::Clock clock;
    float totalTime;

    // Static assets (loaded once, shared by all vehicles)
    static sf::Font font;
    static bool fontLoaded;

    static vector<sf::Texture> carTextures;
    static vector<bool> carTextureLoaded;

    static sf::Texture ambulanceTexture;
    static sf::Texture policeTexture;
    static bool ambulanceLoaded;
    static bool policeLoaded;

    static sf::Texture trafficLightRed;
    static sf::Texture trafficLightGreen;
    static bool redLightLoaded;
    static bool greenLightLoaded;

    // Simulation state
    int nextVehicleID;
    vector<int> vehiclesToRemove;
    bool isRunning;

    // Helper methods
    void loadAssets();
    void initializeSignals();
    void spawnInitialVehicles();
    bool hasSignal(int nodeID) const;

    // Core game loop methods
    void handleInput();
    void update(float deltaTime);
    void render();

    // Input handlers
    void toggleSignal(int signalIndex);
    void spawnRegularVehicle();
    void spawnEmergencyVehicle();
    void undoLastAction();

    // Rendering methods
    void drawRoads();
    void drawIntersections();
    void drawTrafficSignals();
    void drawVehicles();
    void drawHUD();

public:
    Simulation();
    ~Simulation();

    // Main entry point
    void run();

    // Configuration
    bool loadMap(const string &filename);
    void setFrameRate(int fps);

    // Analytics
    void saveAnalytics(const std::string& filename);
    void printAnalytics() const;
};

#endif