#ifndef VEHICLE_H
#define VEHICLE_H

#include <vector>
#include <SFML/Graphics.hpp>
using namespace std;

enum class VehicleStatus
{
    MOVING,
    WAITING,
    ARRIVED,
    EMERGENCY,
};

class Vehicle
{
    int vehicleID;
    vector<int> path;
    int currentPathIndex;
    int currentNode;
    int destNode;
    double speed;
    VehicleStatus status;

public:
    Vehicle(int id, const vector<int>& route, double s = 1.0);
    
    // Movement functions
    void moveToNextNode();  // Advance to next intersection
    void update();  // Called each frame
    
    // Status functions
    bool hasArrivedDest() const;    // Check if at destination
    bool canMove() const;   // Check if allowed to move
    void setStatus(VehicleStatus newStatus);
    
    // Getters
    int getID() const;
    int getCurrentNode() const;
    int getDestNode() const;
    int getNextNode() const;
    VehicleStatus getStatus() const;
    const vector<int>& getPath() const;
    double getSpeed() const;
    
    // For Debugging
    void printInfo() const;
};

#endif