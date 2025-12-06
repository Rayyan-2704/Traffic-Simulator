#include "../include/Vehicle.h"
#include <cmath>
#include <iostream>

Vehicle::Vehicle(int id, const vector<int> &route, double s, int p) : vehicleID(id), speed(s), priority(p)
{
    path = route;
    currentPathIndex = 0;
    interpolation = 0.0f;
    status = (priority > 0) ? VehicleStatus::EMERGENCY : VehicleStatus::MOVING;

    if (path.empty())
    {
        cout << "Warning: Vehicle " << vehicleID << " created with empty path!" << endl;
        status = VehicleStatus::ARRIVED;
        currentNode = -1;
        destNode = -1;
        position = sf::Vector2<float>(0.0f, 0.0f);
        startPosition = position;
        targetPosition = sf::Vector2<float>(0.0f, 0.0f);
    }
    else
    {
        currentNode = path[0];
        destNode = path.back();
        position = sf::Vector2<float>(0.0f, 0.0f);
        startPosition = position;
        targetPosition = sf::Vector2<float>(0.0f, 0.0f);
    }
}

void Vehicle::moveToNextNode(const sf::Vector2<float> &nextNodePos)
{
    // Check if already at destination
    if (currentPathIndex >= path.size() - 1 || (status != VehicleStatus::MOVING && status != VehicleStatus::EMERGENCY))
    {
        return;
    }

    // Move to next node
    currentPathIndex++;
    currentNode = path[currentPathIndex];

    startPosition = position;
    targetPosition = nextNodePos;
    interpolation = 0.0f;

    cout << "Vehicle " << vehicleID << " has moved to Node " << currentNode << endl;
}

void Vehicle::updateVisualPosition(float deltaTime)
{
    if ((status != VehicleStatus::MOVING && status != VehicleStatus::EMERGENCY) || hasArrivedDest())
    {
        return;
    }

    // Increase interpolation (0 → 1)
    interpolation += static_cast<float>(speed) * deltaTime * 0.5f;
    if (interpolation > 1.0f)
    {
        interpolation = 1.0f;
    }

    // Linear interpolation:
    // pos = start*(1-t) + end*t
    position.x = startPosition.x * (1.0f - interpolation) + targetPosition.x * interpolation;
    position.y = startPosition.y * (1.0f - interpolation) + targetPosition.y * interpolation;

    // When interpolation == 1, vehicle has fully reached targetPosition.
    // The main loop will then call moveToNextNode() to go to the next segment.
}

// void Vehicle::update()
// {
//     if (status == VehicleStatus::MOVING){
//         // moveToNextNode();
//     }

//     // if vehicle arrived, do nothing
//     // if vehicle is waiting, do nothing for now (traffic logic to be implemented)
// }

bool Vehicle::hasArrivedDest() const { return status == VehicleStatus::ARRIVED; }

bool Vehicle::canMove() const
{ 
    return (status == VehicleStatus::MOVING || status == VehicleStatus::EMERGENCY) && !hasArrivedDest(); 
}

void Vehicle::setStatus(VehicleStatus newStatus) { status = newStatus; }

int Vehicle::getID() const { return vehicleID; }
int Vehicle::getCurrentNode() const { return currentNode; }
int Vehicle::getDestNode() const { return destNode; }
int Vehicle::getNextNode() const
{
    return (currentPathIndex < path.size() - 1) ? path[currentPathIndex + 1] : -1;
}
VehicleStatus Vehicle::getStatus() const { return status; }
const vector<int> &Vehicle::getPath() const { return path; }
double Vehicle::getSpeed() const { return speed; }
sf::Vector2<float> Vehicle::getPosition() const { return position; }
float Vehicle::getInterpolation() const { return interpolation; }
int Vehicle::getPriority() const { return priority; }
bool Vehicle::isEmergency() const { return priority > 0; }

void Vehicle::setPosition(const sf::Vector2<float> &pos) { position = pos; }
void Vehicle::setStartPosition(const sf::Vector2<float> &start) { startPosition = start; }
void Vehicle::setTargetPosition(const sf::Vector2<float> &target)
{
    targetPosition = target;
    interpolation = 0.0f;
}

void Vehicle::printInfo() const
{
    cout << "\n--- Vehicle " << vehicleID << " Info ---" << endl;
    cout << "Current Node: " << currentNode << endl;
    cout << "Destination Node: " << destNode << endl;
    cout << "Path: ";

    for (int i = 0; i < path.size(); i++)
    {
        cout << path[i];
        if (i < path.size() - 1)
            cout << " -> ";
    }
    cout << endl;

    cout << "Progress: " << currentPathIndex + 1 << "/" << path.size() << " nodes" << endl;
    cout << "Status: ";
    switch (status)
    {
    case VehicleStatus::MOVING:
        cout << "MOVING";
        break;
    case VehicleStatus::WAITING:
        cout << "WAITING";
        break;
    case VehicleStatus::ARRIVED:
        cout << "ARRIVED";
        break;
    case VehicleStatus::EMERGENCY:
        cout << "EMERGENCY";
        break;
    }
    cout << endl;
    cout << "Speed: " << speed << endl;
    cout << "-------------------------\n" << endl;
}