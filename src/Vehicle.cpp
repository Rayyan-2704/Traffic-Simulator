#include "../include/Vehicle.h"
#include "../include/Graph.h"
#include <cmath>
#include <iostream>

Vehicle::Vehicle(int id, const vector<int> &route, double s) : vehicleID(id), speed(s)
{
    path = route;
    currentPathIndex = 0;
    status = VehicleStatus::MOVING;

    if (path.empty())
    {
        cout << "Warning: Vehicle " << vehicleID << " created with empty path!" << endl;
        status = VehicleStatus::ARRIVED;
        currentNode = -1;
        destNode = -1;
    }
    else
    {
        currentNode = path[0];
        destNode = path.back();
    }
}

void Vehicle::moveToNextNode()
{
    // Check if already at destination
    if (currentPathIndex >= path.size() - 1)
    {
        if (status != VehicleStatus::ARRIVED)
        {
            status = VehicleStatus::ARRIVED;
            cout << "Vehicle " << vehicleID << " has arrived at the Destination Node " << destNode << endl;
        }
        return;
    }

    // Can't move if not in MOVING state
    if (status != VehicleStatus::MOVING)
    {
        return;
    }

    // Move to next node
    currentPathIndex++;
    currentNode = path[currentPathIndex];

    cout << "Vehicle " << vehicleID << " has moved to Node " << currentNode << endl;

    // Check if we just arrived at destination
    if (currentPathIndex >= path.size() - 1)
    {
        status = VehicleStatus::ARRIVED;
        cout << "Vehicle " << vehicleID << " has arrived at the Destination Node " << destNode << endl;
    }
}

void Vehicle::update()
{
    if (status == VehicleStatus::MOVING)
        moveToNextNode();

    // if vehicle arrived, do nothing
    // if vehicle is waiting, do nothing for now (traffic logic to be implemented)
}

bool Vehicle::hasArrivedDest() const { return status == VehicleStatus::ARRIVED; }
bool Vehicle::canMove() const { return status == VehicleStatus::MOVING && !hasArrivedDest(); }
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
    cout << "-------------------------\n"
         << endl;
}