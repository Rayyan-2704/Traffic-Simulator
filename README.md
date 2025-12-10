# 🚦 Traffic Simulator - Smart City Traffic Simulation
 A comprehensive traffic simulation system built with C++17 and SFML 3.0, featuring advanced data structures, real-time visualization, and intelligent traffic management algorithms.

---

## 🎯 Overview

The **Traffic Simulator** is a sophisticated city traffic management system that models real-world traffic scenarios using multiple advanced data structures and algorithms. Built as a comprehensive Data Structures & Algorithms project, it demonstrates practical applications of DSA concepts in solving real-world problems.

### Key Highlights

- **Real-time visualization** using SFML 3.0 graphics library
- **10+ Data Structures** implemented from scratch
- **Pathfinding** using Dijkstra's algorithm
- **Priority-based emergency vehicle** routing
- **Interactive traffic signals** with undo functionality
- **Analytics engine** with comprehensive traffic statistics
- **File I/O** for map loading and report generation

---

## ✨ Features

### 🚗 Vehicle Management
- Spawn regular and emergency vehicles dynamically
- Smooth interpolated movement between intersections
- Priority-based routing for ambulances and police vehicles
- Real-time vehicle status tracking (Moving, Waiting, Arrived)

### 🚦 Traffic Signal Control
- **Manual control** for 2 signals (toggle via keyboard)
- **Automatic switching** for 2 signals with configurable timers
- Queue-based vehicle waiting system at red lights
- Emergency vehicles bypass red signals automatically

### ⏮️ Undo System
- Action stack for reverting signal toggles
- Undo vehicle spawns (if not yet arrived)
- Timestamp-based action tracking
- Configurable stack size (default: 100 actions)

### 📊 Analytics & Statistics
- Total vehicles spawned, active, and arrived
- Average travel time calculation
- Road congestion analysis (sorted using Merge Sort)
- Busiest intersection detection
- Signal toggle and undo operation tracking
- Real-time simulation report generation

### 🗂️ File Handling
- **Map loading** from text files (`roads.txt`)
- **Analytics export** to formatted text reports
- Node and edge data persistence
- Simulation statistics logging

---

## 🧩 Data Structures Implemented

This project showcases **10 fundamental data structures**, all implemented from scratch (except STL vectors and unordered_map):

### 1. **Graph** (Adjacency List Representation)
```cpp
class Graph {
    unordered_map<int, vector<Edge>> adjacencyList;
    unordered_map<int, Node> nodes;
};
```
- **Used for:** City road network representation
- **Implementation:** Adjacency list with weighted edges
- **Operations:** O(1) node lookup, O(E) edge traversal

### 2. **Dijkstra's Algorithm** (Shortest Path)
```cpp
vector<int> dijkstraAlgorithm(int start, int end);
```
- **Used for:** Computing optimal routes for vehicles
- **Complexity:** O((V + E) log V) using min-heap priority queue
- **Features:** Early termination, parent tracking for path reconstruction

### 3. **Queue** (Linked List Implementation)
```cpp
template <typename T>
class Queue {
    QueueListNode<T> *start;
    QueueListNode<T> *end;
};
```
- **Used for:** Traffic signal waiting queues
- **Implementation:** Singly linked list with head/tail pointers
- **Operations:** O(1) enqueue/dequeue

### 4. **Stack** (Linked List Implementation)
```cpp
template <typename T>
class Stack {
    StackListNode<T> *Top;
};
```
- **Used for:** Undo action history
- **Implementation:** Singly linked list
- **Operations:** O(1) push/pop/peek

### 5. **Priority Queue - Min Heap** (for Dijkstra)
```cpp
template <typename T>
class MinHeapPQ {
    vector<T> heap;
    void heapifyUp(int index);
    void heapifyDown(int index);
};
```
- **Used for:** Dijkstra's algorithm node selection
- **Implementation:** Array-based binary heap
- **Operations:** O(log n) insert/extract-min

### 6. **Priority Queue - Max Heap** (for Emergency Vehicles)
```cpp
class MaxHeapPQ {
    vector<Vehicle*> heap;
    bool hasHigherPriority(Vehicle* a, Vehicle* b);
};
```
- **Used for:** Emergency vehicle priority management
- **Priority Levels:** 
  - 8-10: Ambulance (highest)
  - 5-7: Police
  - 0: Regular vehicles (not in heap)
- **Operations:** O(log n) insert/extract-max

### 7. **Vector** (STL)
```cpp
vector<Vehicle> vehicles;
vector<TrafficSignal> signals;
```
- **Used for:** Dynamic vehicle and signal storage
- **Features:** Random access, dynamic resizing
- **Operations:** O(1) access, O(1) amortized push_back

### 8. **Unordered Map / HashMap** (STL)
```cpp
unordered_map<string, int> roadUsage;
unordered_map<int, float> vehicleSpawnTimes;
```
- **Used for:** Analytics tracking, node lookups
- **Implementation:** Hash table with chaining
- **Operations:** O(1) average insert/search/delete

### 9. **Merge Sort** (Divide & Conquer)
```cpp
void mergeSort(vector<RoadCongestion>& roads, int left, int right);
void merge(vector<RoadCongestion>& roads, int left, int mid, int right);
```
- **Used for:** Sorting road congestion data
- **Complexity:** O(n log n) guaranteed
- **Features:** Stable sort, descending order by traffic count

### 10. **Struct/Class Hierarchy**
```cpp
struct Node, Edge, Action, RoadCongestion
class Vehicle, TrafficSignal, Simulation, Analytics
```
- **Used for:** Encapsulation and data organization
- **Features:** OOP principles, composition over inheritance

---

## 🏗️ Technical Architecture

### Core Components

```
┌─────────────────────────────────────────────────────────┐
│                   Simulation Engine                     │
│  ┌─────────────┐  ┌──────────────┐  ┌───────────────┐   │
│  │   Graph     │  │   Vehicles   │  │    Signals    │   │
│  │  (City Map) │  │  Management  │  │   Control     │   │
│  └─────────────┘  └──────────────┘  └───────────────┘   │
│         │                │                   │          │
│         └────────────────┼───────────────────┘          │
│                          │                              │
│                          ▼                              │
│  ┌─────────────────────────────────────────────────┐    │
│  │         SFML Rendering & Input Handling         │    │
│  └─────────────────────────────────────────────────┘    │
│                          │                              │
│                          ▼                              │
│  ┌─────────────────────────────────────────────────┐    │
│  │  ActionStack  │  EmergencyManager  │ Analytics  │    │
│  │   (Undo)      │   (PriorityQueue)  │ (Sorting)  │    │
│  └─────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────┘
```

### Class Diagram (Simplified)

```
Simulation
├── Graph (adjacencyList, nodes)
│   └── dijkstraAlgorithm()
├── vector<Vehicle>
│   ├── position, path, priority
│   └── updateVisualPosition()
├── vector<TrafficSignal>
│   ├── Queue<Vehicle*> waitingQueue
│   └── toggleLight()
├── ActionStack
│   └── Stack<Action> history
├── EmergencyManager
│   └── MaxHeapPQ emergencyQueue
└── Analytics
    ├── unordered_map<string, int> roadUsage
    └── mergeSort()
```

---

## 🎮 Usage

### Running the Simulation

1. **Launch the executable**
   ```bash
   ./TrafficSimulator
   ```

2. **Wait for initialization**
   - City map loads from `data/roads.txt`
   - 3 initial vehicles spawn automatically
   - 4 traffic signals initialize (2 manual, 2 automatic)

3. **Interact with the simulation**
   - Toggle signals manually
   - Spawn additional vehicles
   - Use undo to revert actions
   - Watch real-time traffic flow

4. **Exit and view analytics**
   - Press ESC to close
   - Check `data/simulation_log.txt` for detailed report

---

## 🎯 Controls

| Key | Action | Description |
|-----|--------|-------------|
| **1** | Toggle Signal 1 | Manual control of signal at Node 1 |
| **2** | Toggle Signal 2 | Manual control of signal at Node 5 |
| **3** | Toggle Signal 3 | Manual control of signal at Node 6 |
| **4** | Toggle Signal 4 | Manual control of signal at Node 9 |
| **A** | Spawn Car | Add regular vehicle with random route |
| **E** | Spawn Emergency | Add ambulance/police (priority 5-10) |
| **U** | Undo | Revert last action (signal/spawn) |
| **ESC** | Exit | Close simulation and save analytics |

---

## 📁 Project Structure

```
traffic-simulator/
├── assets/
│   ├── fonts/
│   │   └── Orbitron-Bold.ttf
│   └── images/
│       ├── vehicles/
│       │   ├── car_blue.png
│       │   ├── car_red.png
│       │   ├── ambulance.png
│       │   └── police.png
│       └── signals/
│           ├── trafficLight_red.png
│           └── trafficLight_green.png
├── data/
│   ├── roads.txt              # Input: City graph definition
│   └── simulation_log.txt     # Output: Analytics report
├── include/
│   ├── Graph.h                # Graph + Dijkstra
│   ├── Vehicle.h              # Vehicle class
│   ├── TrafficSignal.h        # Signal + Queue
│   ├── Queue.h                # Generic Queue (linked list)
│   ├── Stack.h                # Generic Stack (linked list)
│   ├── PriorityQueue.h        # Min/Max Heap implementations
│   ├── ActionStack.h          # Undo system
│   ├── EmergencyManager.h     # Emergency vehicle priority
│   ├── Analytics.h            # Statistics + Merge Sort
│   └── Simulation.h           # Main simulation engine
├── src/
│   ├── Graph.cpp
│   ├── Vehicle.cpp
│   ├── TrafficSignal.cpp
│   ├── ActionStack.cpp
│   ├── EmergencyManager.cpp
│   ├── Analytics.cpp
│   ├── Simulation.cpp
│   └── main.cpp
├── .vscode/
│   ├── tasks.json             # Build configuration
│   ├── c_cpp_properties.json  # IntelliSense config
│   └── settings.json
├── README.md
└── LICENSE
```

---

## 📊 Analytics & Logging

### Sample Report (`simulation_log.txt`)

```
========================================
    TRAFFIC SIMULATION REPORT
========================================

SIMULATION DURATION: 141.55 seconds

--- VEHICLE STATISTICS ---
Total Vehicles Spawned:    362
Currently Active:          0
Successfully Arrived:      356
Emergency Vehicles:        170
Currently Queued:          5
Completed Trips:           356
Average Travel Time:       7.50 seconds

--- TRAFFIC FLOW ---
Average Queue Size:        0.00 vehicles
Busiest Road:              2 -> 1 (71 vehicles)
Total Road Segments Used:  42

--- USER INTERACTIONS ---
Signal Toggles:            18
Undo Operations:           14

--- ROAD CONGESTION (Sorted by Traffic) ---
1. Road 2 -> 1: 71 vehicles
2. Road 0 -> 1: 67 vehicles
3. Road 1 -> 2: 41 vehicles
4. Road 1 -> 5: 40 vehicles
5. Road 8 -> 9: 40 vehicles
6. Road 3 -> 2: 38 vehicles
7. Road 7 -> 11: 32 vehicles
8. Road 10 -> 6: 25 vehicles
9. Road 4 -> 5: 25 vehicles
10. Road 4 -> 0: 24 vehicles

========================================
Report generated successfully
========================================
```

### Tracked Metrics

- **Vehicle Statistics:** Spawn count, arrivals, active vehicles
- **Travel Metrics:** Average travel time per vehicle
- **Traffic Flow:** Queue sizes, road usage, congestion hotspots
- **User Actions:** Signal toggles, undo operations
- **Road Analysis:** Merge-sorted congestion data

---

## 👥 Authors

- [Rayyan Aamir](https://github.com/Rayyan-2704)
- [Muhammad Usaid Khan](https://github.com/MuhammadUsaidKhan)
- [Syed Hammad Haider](https://github.com/Syed-Hammad-Haider)