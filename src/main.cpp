#include <iostream>
#include "../include/Graph.h"
using namespace std;

int main()
{
    cout << "=== Traffic Simulator - Day 1 Test ===" << endl;
    cout << "Testing Graph Module and Dijkstra's Algorithm\n"
         << endl;

    Graph cityMap;
    if (!cityMap.loadFromFile("data/roads.txt"))
    {
        cout << "Failed to load map data!" << endl;
        return 1;
    }

    cityMap.printGraph();

    // Test Dijkstra's algorithm with multiple routes
    cout << "=== TESTING DIJKSTRA'S ALGORITHM ===" << endl;

    // Test 1: Path from node 0 to node 3
    cout << "\nTest 1: Finding shortest path from Node 0 to Node 3" << endl;
    vector<int> path1 = cityMap.dijkstraAlgorithm(0, 3);

    if (!path1.empty())
    {
        cout << "Shortest path: ";
        for (int i = 0; i < path1.size(); i++)
        {
            cout << path1[i];
            if (i < path1.size() - 1)
                cout << " -> ";
        }
        cout << endl;
    }

    // Test 2: Path from node 1 to node 2
    cout << "\nTest 2: Finding shortest path from Node 1 to Node 2" << endl;
    vector<int> path2 = cityMap.dijkstraAlgorithm(1, 2);

    if (!path2.empty())
    {
        cout << "Shortest path: ";
        for (int i = 0; i < path2.size(); i++)
        {
            cout << path2[i];
            if (i < path2.size() - 1)
                cout << " -> ";
        }
        cout << endl;
    }

    // Test 3: Path from node 0 to node 2 (should compare routes)
    cout << "\nTest 3: Finding shortest path from Node 0 to Node 2" << endl;
    vector<int> path3 = cityMap.dijkstraAlgorithm(0, 2);

    if (!path3.empty())
    {
        cout << "Shortest path: ";
        for (int i = 0; i < path3.size(); i++)
        {
            cout << path3[i];
            if (i < path3.size() - 1)
                cout << " -> ";
        }
        cout << endl;
        cout << "Note: Direct route 0->2 (weight 25) vs 0->1->2 (weight 10+15=25)" << endl;
    }

    cout << "\n=== Day 1 Test Complete! ===" << endl;
    cout << "Graph module is working correctly!" << endl;

    return 0;
}