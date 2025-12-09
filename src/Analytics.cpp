#include "../include/Analytics.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>

Analytics::Analytics()
{
    cout << "Analytics system initialized" << endl;
}

string Analytics::getRoadKey(int from, int to) const
{
    return to_string(from) + "-" + to_string(to);
}

void Analytics::recordVehicleSpawn(int vehicleID, float time, bool isEmergency)
{
    vehicleSpawnTimes[vehicleID] = time;
    stats.totalVehiclesSpawned++;

    if (isEmergency)
        stats.totalEmergencyVehicles++;
}

void Analytics::recordVehicleArrival(int vehicleID, float time)
{
    vehicleArrivalTimes[vehicleID] = time;
    stats.totalArrivedVehicles++;
}

void Analytics::recordRoadUsage(int fromNode, int toNode)
{
    string key = getRoadKey(fromNode, toNode);
    roadUsage[key]++;
}

void Analytics::recordSignalToggle()
{
    stats.totalSignalToggles++;
}

void Analytics::recordUndoOperation()
{
    stats.totalUndoOperations++;
}

void Analytics::merge(vector<RoadCongestion> &roads, int low, int mid, int high)
{
    int left = low;
    int right = mid + 1;

    vector<RoadCongestion> temp(high - low + 1);
    int idx = 0;

    // Merge two sorted halves (descending order of vehicleCount)
    while (left <= mid && right <= high)
    {
        if (roads[left].vehicleCount >= roads[right].vehicleCount)
            temp[idx++] = roads[left++];
        else
            temp[idx++] = roads[right++];
    }

    while (left <= mid)
        temp[idx++] = roads[left++];

    while (right <= high)
        temp[idx++] = roads[right++];

    // Copy temp back into original vector
    for (int i = low; i <= high; i++)
        roads[i] = temp[i - low];
}

void Analytics::mergeSort(vector<RoadCongestion> &roads, int left, int right)
{
    if (left >= right)
        return;

    int mid = left + (right - left) / 2;

    mergeSort(roads, left, mid);
    mergeSort(roads, mid + 1, right);

    merge(roads, left, mid, right);
}

vector<RoadCongestion> Analytics::getCongestionData() const
{
    vector<RoadCongestion> congestion;

    for (const auto &pair : roadUsage)
    {
        // Parse "from-to" key
        size_t dashPos = pair.first.find('-');
        int from = stoi(pair.first.substr(0, dashPos));
        int to = stoi(pair.first.substr(dashPos + 1));

        congestion.push_back(RoadCongestion(from, to, pair.second));
    }

    return congestion;
}

vector<RoadCongestion> Analytics::getSortedCongestionData()
{
    vector<RoadCongestion> congestion = getCongestionData();

    if (!congestion.empty())
    {
        cout << "\n=== SORTING CONGESTION DATA (Merge Sort) ===" << endl;
        cout << "Unsorted road count: " << congestion.size() << endl;

        mergeSort(congestion, 0, congestion.size() - 1);

        cout << "Sorted! Busiest road: " << congestion[0].fromNode << " -> " << congestion[0].toNode << " (" << congestion[0].vehicleCount << " vehicles)" << endl;
    }

    return congestion;
}

RoadCongestion Analytics::getBusiestRoad()
{
    auto sortedRoads = getSortedCongestionData();

    if (sortedRoads.empty())
        return RoadCongestion(-1, -1, 0);

    return sortedRoads[0]; // First element is busiest after sorting
}

void Analytics::updateStats(const vector<Vehicle> &vehicles, const vector<TrafficSignal> &signals, float currentTime)
{
    stats.simulationDuration = currentTime;

    // Count active vehicles
    int active = 0, arrived = 0, queued = 0, emergency = 0;;
    for (const auto &vehicle : vehicles)
    {
        if (vehicle.hasArrivedDest())
            arrived++;
        else if (vehicle.getStatus() == VehicleStatus::WAITING)
            queued++;
        else if (vehicle.isEmergency())
        {
            emergency++;
            active++;
        }
        else
            active++;
        
        if (vehicle.isEmergency())
            emergency++;
    }

    stats.currentActiveVehicles = active;
    stats.totalArrivedVehicles = arrived;
    stats.totalQueuedVehicles = queued;
    stats.totalEmergencyVehicles = emergency;

    // Calculate average queue size
    int totalQueueSize = 0;
    for (const auto &signal : signals)
    {
        totalQueueSize += signal.getQueueSize();
    }
    if (!signals.empty())
        stats.averageQueueSize = static_cast<double>(totalQueueSize) / signals.size();

    // Calculate average travel time
    double totalTravelTime = 0.0;
    int completedTrips = 0;

    for (const auto &pair : vehicleArrivalTimes)
    {
        int vehicleID = pair.first;
        float arrivalTime = pair.second;

        if (vehicleSpawnTimes.count(vehicleID) > 0)
        {
            float spawnTime = vehicleSpawnTimes[vehicleID];
            totalTravelTime += (arrivalTime - spawnTime);
            completedTrips++;
        }
    }

    stats.averageTravelTime = completedTrips > 0 ? totalTravelTime / completedTrips : 0.0;

    // Find busiest road
    if (!roadUsage.empty())
    {
        RoadCongestion busiest = getBusiestRoad();
        if (busiest.fromNode != -1)
        {
            stats.busiestRoad = to_string(busiest.fromNode) + " -> " + to_string(busiest.toNode);
            stats.busiestRoadCount = busiest.vehicleCount;
        }
    }
}

SimulationStats Analytics::getStats() const { return stats; }
double Analytics::getAverageTravelTime() const { return stats.averageTravelTime; }

bool Analytics::saveToFile(const string &filename)
{
    ofstream file(filename);

    if (!file.is_open())
    {
        cerr << "Error: Could not open file " << filename << endl;
        return false;
    }

    file << "========================================" << endl;
    file << "    TRAFFIC SIMULATION REPORT" << endl;
    file << "========================================" << endl;
    file << endl;

    file << "SIMULATION DURATION: " << fixed << setprecision(2) << stats.simulationDuration << " seconds" << endl;
    file << endl;

    file << "--- VEHICLE STATISTICS ---" << endl;
    file << "Total Vehicles Spawned:    " << stats.totalVehiclesSpawned << endl;
    file << "Currently Active:          " << stats.currentActiveVehicles << endl;
    file << "Successfully Arrived:      " << stats.totalArrivedVehicles << endl;
    file << "Emergency Vehicles:        " << stats.totalEmergencyVehicles << endl;
    file << "Currently Queued:          " << stats.totalQueuedVehicles << endl;
    file << "Completed Trips:           " << vehicleArrivalTimes.size() << endl;
    file << "Average Travel Time:       " << fixed << setprecision(2) << stats.averageTravelTime << " seconds" << endl;
    file << endl;

    file << "--- TRAFFIC FLOW ---" << endl;
    file << "Average Queue Size:        " << fixed << setprecision(2) << stats.averageQueueSize << " vehicles" << endl;
     if (stats.busiestRoadCount > 0)
    {
        file << "Busiest Road:              " << stats.busiestRoad << " (" << stats.busiestRoadCount << " vehicles)" << endl;
    }
    else
    {
        file << "Busiest Road:              No traffic data collected" << endl;
    }
    
    file << "Total Road Segments Used:  " << roadUsage.size() << endl;
    file << endl;

    file << "--- USER INTERACTIONS ---" << endl;
    file << "Signal Toggles:            " << stats.totalSignalToggles << endl;
    file << "Undo Operations:           " << stats.totalUndoOperations << endl;
    file << endl;

    file << "--- ROAD CONGESTION (Sorted by Traffic) ---" << endl;
    auto sortedRoads = getSortedCongestionData();

    for (int i = 0; i < sortedRoads.size() && i < 10; i++) // Top 10 roads
    {
        const auto &road = sortedRoads[i];
        file << (i + 1) << ". Road " << road.fromNode << " -> " << road.toNode << ": " << road.vehicleCount << " vehicles" << endl;
    }

    file << endl;
    file << "========================================" << endl;
    file << "Report generated successfully" << endl;
    file << "========================================" << endl;

    file.close();

    cout << "\n Analytics saved to: " << filename << endl;
    return true;
}

void Analytics::printSummary() const
{
    cout << "\n========================================" << endl;
    cout << "    SIMULATION SUMMARY" << endl;
    cout << "========================================" << endl;
    cout << "Duration: " << fixed << setprecision(1) << stats.simulationDuration << "s" << endl;
    cout << "Vehicles Spawned: " << stats.totalVehiclesSpawned << endl;
    cout << "Vehicles Arrived: " << stats.totalArrivedVehicles << endl;
    cout << "Emergency Vehicles: " << stats.totalEmergencyVehicles << endl;
    cout << "Avg Travel Time: " << fixed << setprecision(2) << stats.averageTravelTime << "s" << endl;
    cout << "Busiest Road: " << stats.busiestRoad << " (" << stats.busiestRoadCount << " vehicles)" << endl;
    cout << "Signal Toggles: " << stats.totalSignalToggles << endl;
    cout << "Undo Operations: " << stats.totalUndoOperations << endl;
    cout << "========================================\n"
         << endl;
}

void Analytics::clear()
{
    roadUsage.clear();
    vehicleSpawnTimes.clear();
    vehicleArrivalTimes.clear();
    stats = SimulationStats();

    cout << "Analytics data cleared" << endl;
}