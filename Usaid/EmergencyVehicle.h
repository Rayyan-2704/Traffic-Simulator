#ifndef EMERGENCYVEHICLE_H
#define EMERGENCYVEHICLE_H

#include "Vehicle.h"
#include <iostream>
using namespace std;

enum class EmergencyType
{
    AMBULANCE = 1,
    FIRE_TRUCK = 2,
    POLICE = 3
};

class EmergencyVehicle : public Vehicle
{
    EmergencyType emergencyType;
    int priority; // Lower number = higher priority (1 is highest)

public:
    EmergencyVehicle(int id, const vector<int> &route, EmergencyType type, double s = 2.0);

    // Getters
    EmergencyType getEmergencyType() const;
    int getPriority() const;
    string getEmergencyTypeName() const;

    // Override for priority comparison (for heap)
    bool operator<(const EmergencyVehicle &other) const;
    bool operator>(const EmergencyVehicle &other) const;

    // For debugging
    void printEmergencyInfo() const;
};

#endif
