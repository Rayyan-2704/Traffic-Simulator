#include "../include/EmergencyVehicle.h"

EmergencyVehicle::EmergencyVehicle(int id, const vector<int> &route, EmergencyType type, double s)
    : Vehicle(id, route, s), emergencyType(type)
{
    // Set priority based on emergency type
    priority = static_cast<int>(type);

    // Emergency vehicles move faster
    setStatus(VehicleStatus::EMERGENCY);

    cout << "Emergency Vehicle " << id << " (" << getEmergencyTypeName()
         << ") created with priority " << priority << endl;
}

EmergencyType EmergencyVehicle::getEmergencyType() const
{
    return emergencyType;
}

int EmergencyVehicle::getPriority() const
{
    return priority;
}

string EmergencyVehicle::getEmergencyTypeName() const
{
    switch (emergencyType)
    {
    case EmergencyType::AMBULANCE:
        return "AMBULANCE";
    case EmergencyType::FIRE_TRUCK:
        return "FIRE TRUCK";
    case EmergencyType::POLICE:
        return "POLICE";
    default:
        return "UNKNOWN";
    }
}

bool EmergencyVehicle::operator<(const EmergencyVehicle &other) const
{
    // For min-heap: lower priority number = higher priority
    return priority > other.priority;
}

bool EmergencyVehicle::operator>(const EmergencyVehicle &other) const
{
    return priority < other.priority;
}

void EmergencyVehicle::printEmergencyInfo() const
{
    cout << "\n=== EMERGENCY VEHICLE INFO ===" << endl;
    printInfo();
    cout << "Type: " << getEmergencyTypeName() << endl;
    cout << "Priority: " << priority << " (1=Highest)" << endl;
    cout << "=============================\n"
         << endl;
}
