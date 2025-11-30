#include "../include/EmergencyManager.h"
#include <iostream>

EmergencyManager::EmergencyManager() : nextEmergencyID(1000)
{
    cout << "EmergencyManager initialized" << endl;
}

void EmergencyManager::addEmergency(Vehicle *vehicle)
{
    if (vehicle == nullptr)
    {
        cout << "Error: Cannot add null vehicle to emergency queue!" << endl;
        return;
    }

    if (vehicle->getPriority() == 0)
    {
        cout << "Error: Cannot add non-emergency vehicle (priority 0) to emergency queue!" << endl;
        return;
    }

    emergencyQueue.push(vehicle);
    cout << "Emergency Vehicle " << vehicle->getID() << " added to queue (Priority: " << vehicle->getPriority() << ", Queue size: " << emergencyQueue.size() << ")" << endl;
}

Vehicle *EmergencyManager::getNextEmergency()
{
    if (emergencyQueue.empty())
        return nullptr;

    return emergencyQueue.top();
}

Vehicle *EmergencyManager::dispatchEmergency()
{
    if (emergencyQueue.empty())
    {
        cout << "No emergency vehicles to dispatch!" << endl;
        return nullptr;
    }

    Vehicle *vehicle = emergencyQueue.top();
    emergencyQueue.pop();

    cout << "Emergency Vehicle " << vehicle->getID() << " dispatched (Priority: " << vehicle->getPriority() << ", Remaining: " << emergencyQueue.size() << ")" << endl;
    return vehicle;
}

bool EmergencyManager::hasEmergencies() const { return !emergencyQueue.empty(); }
int EmergencyManager::getEmergencyCount() const { return emergencyQueue.size(); }

void EmergencyManager::clear()
{
    while (!emergencyQueue.empty())
        emergencyQueue.pop();

    cout << "Emergency queue cleared" << endl;
}

void EmergencyManager::printQueue() const
{
    if (emergencyQueue.empty())
    {
        cout << "Emergency queue is empty" << endl;
        return;
    }

    cout << "\n=== EMERGENCY QUEUE ===" << endl;
    cout << "Total emergencies: " << emergencyQueue.size() << endl;

    cout << "Highest priority vehicle: ID " << emergencyQueue.top()->getID() << " (Priority: " << emergencyQueue.top()->getPriority() << ")" << endl;
    cout << "=======================\n" << endl;
}