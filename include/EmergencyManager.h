#ifndef EMERGENCYMANAGER_H
#define EMERGENCYMANAGER_H

#include <queue>
#include <vector>
#include "Vehicle.h"
#include "PriorityQueue.h"
using namespace std;

// Comparator for priority queue (higher priority -> higher urgency)
struct ComparePriority
{
    bool operator()(Vehicle *a, Vehicle *b) const
    {
        return a->getPriority() < b->getPriority();
    }
};

class EmergencyManager
{
private:
    MaxHeapPQ emergencyQueue;

public:
    EmergencyManager();

    // Core Priority Queue Operations
    void addEmergency(Vehicle *vehicle);
    Vehicle *getNextEmergency();  // Get highest priority vehicle without removing
    Vehicle *dispatchEmergency(); // Get and remove highest priority vehicle

    // Utility Functions
    bool hasEmergencies() const;
    int getEmergencyCount() const;
    void clear();

    // For debugging
    void printQueue() const;
};

#endif