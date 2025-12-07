#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <vector>
#include <string>
#include <unordered_map>
#include "Vehicle.h"
#include "TrafficSignal.h"
#include "Graph.h"
using namespace std;

struct RoadCongestion
{
    int fromNode;
    int toNode;
    int vehicleCount;
    double averageSpeed;

    RoadCongestion() : fromNode(0), toNode(0), vehicleCount(0), averageSpeed(0.0) {}
    RoadCongestion(int from, int to, int count = 0, double speed = 0.0) : fromNode(from), toNode(to), vehicleCount(count), averageSpeed(speed) {}
};

struct SimulationStats
{
    int totalVehiclesSpawned;
    int currentActiveVehicles;
    int totalArrivedVehicles;
    int totalEmergencyVehicles;

    double averageTravelTime;
    double simulationDuration;

    int totalSignalToggles;
    int totalUndoOperations;

    string busiestRoad;
    int busiestRoadCount;

    int totalQueuedVehicles;
    double averageQueueSize;

    SimulationStats() : totalVehiclesSpawned(0), currentActiveVehicles(0), totalArrivedVehicles(0), totalEmergencyVehicles(0), averageTravelTime(0.0), simulationDuration(0.0), totalSignalToggles(0), totalUndoOperations(0), busiestRoadCount(0), totalQueuedVehicles(0), averageQueueSize(0.0) {}
};

class Analytics
{
    // Road usage tracking
    unordered_map<string, int> roadUsage; // "from-to" -> count

    // Statistics
    SimulationStats stats;

    // Vehicle spawn/arrival times
    unordered_map<int, float> vehicleSpawnTimes;
    unordered_map<int, float> vehicleArrivalTimes;

    // Merge sort implementation
    void mergeSort(vector<RoadCongestion> &roads, int left, int right);
    void merge(vector<RoadCongestion> &roads, int left, int mid, int right);

    // Create road key
    string getRoadKey(int from, int to) const;

public:
    Analytics();

    // Track events
    void recordVehicleSpawn(int vehicleID, float time, bool isEmergency);
    void recordVehicleArrival(int vehicleID, float time);
    void recordRoadUsage(int fromNode, int toNode);
    void recordSignalToggle();
    void recordUndoOperation();

    // Analysis methods
    vector<RoadCongestion> getCongestionData() const;
    vector<RoadCongestion> getSortedCongestionData(); // Uses merge sort
    RoadCongestion getBusiestRoad();

    // Statistics
    void updateStats(const vector<Vehicle> &vehicles, const vector<TrafficSignal> &signals, float currentTime);

    SimulationStats getStats() const;
    double getAverageTravelTime() const;

    // File operations
    bool saveToFile(const string &filename);
    void printSummary() const;

    // Reset
    void clear();
};

#endif