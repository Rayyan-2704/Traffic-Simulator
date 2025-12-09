#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <unordered_map>
#include <string>
#include <limits>
#include <SFML/System/Vector2.hpp>
#include "PriorityQueue.h"
using namespace std;

struct Edge
{
    int destination; // Acts like a pointer to which node this edge/road leads to
    double weight;   // How much distance to travel via this edge/road

    Edge(int dest, double w = 0.0) : destination(dest), weight(w) {}
};

struct Node
{
    int ID;
    sf::Vector2<float> position;

    Node(int nodeID = 0, float posX = 0.0f, double posY = 0.0f) : ID(nodeID), position(posX, posY) {}

    float getX() const { return position.x; }
    float getY() const { return position.y; }
};

class Graph
{
    int numNodes;
    unordered_map<int, vector<Edge>> adjacencyList;
    unordered_map<int, Node> nodes;

public:
    Graph(int n = 0);

    // Core Graph Operations
    void addEdge(int from, int to, double weight, bool bidirect = true);
    void addNode(int id, float x = 0.0f, float y = 0.0f);

    // Finding shortest path using Dijkstra Algorithm
    vector<int> dijkstraAlgorithm(int start, int end);

    // Graph Utility Functions
    bool nodeExists(int id) const;
    const vector<Edge> &getNeighbours(int id) const;
    const Node &getNode(int id) const;
    int getNumNodes() const;
    const unordered_map<int, Node> &getAllNodes() const;

    // File I/O
    bool loadFromFile(const string &fileName);
    // For Debugging
    void printGraph() const;
};

#endif