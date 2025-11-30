#ifndef VEHICLE_H
#define VEHICLE_H

#include <vector>
#include <SFML/System/Vector2.hpp>
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
    int priority; // 0 for regular cars, above 0 for emergency vehicles

    sf::Vector2<float> position;       // current position
    sf::Vector2<float> startPosition;  // position of the node we are starting from
    sf::Vector2<float> targetPosition; // position of the node we are moving towards
    float interpolation;

public:
    Vehicle(int id, const vector<int> &route, double s = 1.0, int p = 0);

    // Movement functions
    void moveToNextNode(const sf::Vector2<float> &nextNodePos); // Advance to next intersection
    // void update();  // Called each frame
    void updateVisualPosition(float deltaTime); // For smooth animation

    // Status functions
    bool hasArrivedDest() const; // Check if at destination
    bool canMove() const;        // Check if allowed to move
    void setStatus(VehicleStatus newStatus);

    // Getters
    int getID() const;
    int getCurrentNode() const;
    int getDestNode() const;
    int getNextNode() const;
    VehicleStatus getStatus() const;
    const vector<int> &getPath() const;
    double getSpeed() const;
    sf::Vector2<float> getPosition() const;
    float getInterpolation() const;
    int getPriority() const;

    // Setters
    void setPosition(const sf::Vector2<float> &pos);
    void setStartPosition(const sf::Vector2<float> &start);
    void setTargetPosition(const sf::Vector2<float> &target);

    // For Debugging
    void printInfo() const;
};

#endif