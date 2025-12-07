#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <vector>
#include <unordered_map>
#include <string>
#include <utility>
#include "Vehicle.h"
#include "TrafficSignal.h"
using namespace std;

// Structure to store road traffic data
struct RoadTraffic
{
    int fromNode;
    int toNode;
    int vehicleCount;
    double totalWeight;

    RoadTraffic(int from, int to, int count = 0, double weight = 0.0)
        : fromNode(from), toNode(to), vehicleCount(count), totalWeight(weight) {}

    // For sorting by traffic count (descending)
    bool operator<(const RoadTraffic &other) const
    {
        return vehicleCount > other.vehicleCount; // Descending order
    }
};

// Structure to store intersection congestion data
struct IntersectionData
{
    int nodeID;
    int totalVehiclesPassed;
    int currentQueueSize;
    bool hasSignal;

    IntersectionData(int id = 0, int passed = 0, int queue = 0, bool signal = false)
        : nodeID(id), totalVehiclesPassed(passed), currentQueueSize(queue), hasSignal(signal) {}

    bool operator<(const IntersectionData &other) const
    {
        return totalVehiclesPassed > other.totalVehiclesPassed; // Descending order
    }
};

class Analytics
{
private:
    // Traffic statistics
    int totalCarsSpawned;
    int totalEmergencyVehicles;
    int totalArrivedCars;
    int totalSignalToggles;
    int totalUndoOperations;
    float simulationStartTime;
    float simulationEndTime;

    // Road traffic tracking (using pair<from, to> as key)
    unordered_map<string, int> roadTrafficCount;

    // Travel time tracking
    unordered_map<int, float> vehicleSpawnTime;
    unordered_map<int, float> vehicleArrivalTime;

    // Intersection tracking
    unordered_map<int, int> intersectionTraffic;

    // Helper function to generate road key
    string getRoadKey(int from, int to) const;

    // Sorting algorithms (implemented from scratch)
    void mergeSort(vector<RoadTraffic> &arr, int left, int right);
    void merge(vector<RoadTraffic> &arr, int left, int mid, int right);

    void mergeSortIntersections(vector<IntersectionData> &arr, int left, int right);
    void mergeIntersections(vector<IntersectionData> &arr, int left, int mid, int right);

public:
    Analytics();

    // Recording events
    void recordVehicleSpawn(int vehicleID, float currentTime, bool isEmergency);
    void recordVehicleArrival(int vehicleID, float currentTime);
    void recordRoadUsage(int fromNode, int toNode);
    void recordSignalToggle();
    void recordUndo();
    void recordIntersectionPass(int nodeID);

    // Analysis functions
    vector<RoadTraffic> getBusiestRoads(int topN = 5);
    vector<IntersectionData> getBusiestIntersections(const vector<TrafficSignal> &signals, int topN = 5);
    double getAverageTravelTime() const;
    int getTotalCarsSpawned() const;
    int getTotalEmergencyVehicles() const;
    int getTotalArrivedCars() const;
    float getSimulationDuration() const;

    // Setters
    void setSimulationStartTime(float time);
    void setSimulationEndTime(float time);

    // Statistics display
    void printStatistics() const;
    void printBusiestRoads(int topN = 5);
    void printBusiestIntersections(const vector<TrafficSignal> &signals, int topN = 3);

    // Get formatted stats (for file logging)
    string getFormattedStats() const;
    string getFormattedBusiestRoads(int topN = 5);
    string getFormattedBusiestIntersections(const vector<TrafficSignal> &signals, int topN = 3);
};

#endif
