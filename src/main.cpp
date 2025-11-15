#include <iostream>
#include <vector>
#include "../include/Graph.h"
#include "../include/Vehicle.h"
using namespace std;

int main()
{
    cout << "=== Traffic Simulator - Day 2 Test ===" << endl;
    cout << "Testing Vehicle Class and Movement\n" << endl;

    Graph cityMap;
    if (!cityMap.loadFromFile("data/roads.txt"))
    {
        cout << "Failed to load map data!" << endl;
        return 1;
    }
    cout << "City map loaded with " << cityMap.getNumNodes() << " nodes\n" << endl;

    cout << "=== SPAWNING VEHICLES ===" << endl;

    // Car 1: Short trip (0 -> 2)
    vector<int> path1 = cityMap.dijkstraAlgorithm(0, 2);
    Vehicle car1(1, path1, 1.0);
    cout << "Car #1 spawned: Route from Node 0 to Node 2" << endl;

    // Car 2: Medium trip (1 -> 7)
    vector<int> path2 = cityMap.dijkstraAlgorithm(1, 7);
    Vehicle car2(2, path2, 1.5);
    cout << "Car #2 spawned: Route from Node 1 to Node 7" << endl;

    // Car 3: Long trip (0 -> 11)
    vector<int> path3 = cityMap.dijkstraAlgorithm(0, 11);
    Vehicle car3(3, path3, 0.8);
    cout << "Car #3 spawned: Route from Node 0 to Node 11" << endl;

    vector<Vehicle> allCars;
    allCars.push_back(car1);
    allCars.push_back(car2);
    allCars.push_back(car3);

    cout << "\nTotal cars in simulation: " << allCars.size() << endl;

    cout << "\n=== INITIAL VEHICLE INFO ===" << endl;
    for (auto& car : allCars) {
        car.printInfo();
    }

    // Simulate movement for several ticks
    cout << "\n=== SIMULATION START ===" << endl;
    cout << "Simulating 10 time steps...\n" << endl;

    for (int tick = 1; tick <= 10; tick++) {
        cout << "--- Tick " << tick << " ---" << endl;
        
        // Update all vehicles
        for (auto& car : allCars) {
            if (!car.hasArrivedDest()) {
                car.update();
            }
        }
        
        // Show status summary
        int moving = 0, arrived = 0;
        for (const auto& car : allCars) {
            if (car.hasArrivedDest()) arrived++;
            else moving++;
        }
        cout << "Status: " << moving << " moving, " << arrived << " arrived" << endl;
        cout << endl;
        
        // Stop if all cars arrived
        if (arrived == allCars.size()) {
            cout << "All vehicles have reached their destinations!" << endl;
            break;
        }
    }

    // Final status
    cout << "\n=== FINAL VEHICLE INFO ===" << endl;
    for (auto& car : allCars) {
        car.printInfo();
    }

    cout << "\n=== Day 2 Test Complete! ===" << endl;
    cout << "Vehicle class is working correctly!" << endl;

    return 0;
}