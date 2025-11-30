#include "../include/Graph.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

Graph::Graph(int n) : numNodes(n)
{
    if (n > 0)
    {
        for (int i = 0; i < n; i++)
        {
            nodes[i] = Node(i);
        }
    }
}

void Graph::addEdge(int from, int to, double weight, bool bidirect)
{
    if (from >= numNodes || from < 0 || to >= numNodes || to < 0)
    {
        cout << "Error! Invalid node entered in addEdge()" << endl;
        return;
    }

    adjacencyList[from].push_back(Edge(to, weight));

    if (bidirect)
        adjacencyList[to].push_back(Edge(from, weight));
}

void Graph::addNode(int id, float x, float y)
{
    nodes[id] = Node(id, x, y);
}

vector<int> Graph::dijkstraAlgorithm(int start, int end)
{
    if (!nodeExists(start) || !nodeExists(end))
    {
        cout << "Error: Start or end node doesn't exist" << endl;
        return {};
    }

    // Distance array initialized to large value (infinity)
    unordered_map<int, double> dist;
    unordered_map<int, int> parent; // To reconstruct path

    // Initialize all distances to infinity
    for (const auto &nodePair : nodes)
    {
        dist[nodePair.first] = numeric_limits<double>::infinity();
        parent[nodePair.first] = -1;
    }

    // Distance to source is 0
    dist[start] = 0;

    // Min-heap storing {distance, node}
    MinHeapPQ<pair<double, int>> pq;

    // Push source into heap
    pq.push({0, start});

    // Process nodes until heap is empty
    while (!pq.empty())
    {
        // Extract node with minimum distance
        double d = pq.top().first;
        int node = pq.top().second;
        pq.pop();

        // Skip if this distance is outdated
        if (d > dist[node])
            continue;

        // Early exit if we reached destination (optimization)
        if (node == end)
            break;

        // Traverse all adjacent neighbors
        for (const Edge &edge : getNeighbours(node))
        {
            int next = edge.destination;
            double wt = edge.weight;

            // Relaxation check
            if (dist[node] + wt < dist[next])
            {
                // Update distance
                dist[next] = dist[node] + wt;

                // Update parent (for path reconstruction)
                parent[next] = node;

                // Push updated distance into heap
                pq.push({dist[next], next});
            }
        }
    }

    // Reconstruct path from end to start using parent pointers
    vector<int> path;
    if (dist[end] == numeric_limits<double>::infinity())
    {
        cout << "No path found from " << start << " to " << end << endl;
        return path;
    }

    int current = end;
    while (current != -1)
    {
        path.push_back(current);
        current = parent[current];
    }

    reverse(path.begin(), path.end());
    return path;
}

bool Graph::nodeExists(int id) const
{
    return nodes.find(id) != nodes.end();
}

const vector<Edge> &Graph::getNeighbours(int id) const
{
    static const vector<Edge> empty;
    if (!nodeExists(id))
    {
        cout << "Error: Node " << id << " doesn't exist!" << endl;
        return empty;
    }

    auto iter = adjacencyList.find(id);
    if (iter != adjacencyList.end())
        return iter->second;

    return empty; // returning empty vector only if the node does not exist
}

const Node &Graph::getNode(int id) const
{
    static const Node invalidNode(-1);

    if (!nodeExists(id))
    {
        cout << "Error: Node " << id << " doesn't exist!" << endl;
        return invalidNode;
    }

    auto iter = nodes.find(id);
    if (iter != nodes.end())
        return iter->second;

    return invalidNode;
}

int Graph::getNumNodes() const { return numNodes; }

const unordered_map<int, Node> &Graph::getAllNodes() const { return nodes; }

bool Graph::loadFromFile(const string &fileName)
{
    ifstream file(fileName);
    if (!file.is_open())
    {
        cout << "Error in opening file " << fileName << endl;
        return false;
    }

    file >> numNodes;

    nodes.clear();
    adjacencyList.clear();

    for (int i = 0; i < numNodes; i++)
    {
        // Default positions in a grid (adjusted for 800x600 SFML window)
        float x = (i % 4) * 200.0f + 100.0f; // 4 columns
        float y = (i / 4) * 180.0f + 80.0f;  // 3 rows
        nodes[i] = Node(i, x, y);
    }

    int from, to;
    double weight;
    while (file >> from >> to >> weight)
    {
        if (from >= numNodes || to >= numNodes || from < 0 || to < 0)
        {
            cout << "Invalid edge in file: " << from << " " << to << endl;
            continue;
        }
        addEdge(from, to, weight, true); // bidirectional roads
    }

    file.close();
    cout << "Graph loaded successfully! Total number of nodes: " << numNodes << endl;
    return true;
}

void Graph::printGraph() const
{
    cout << "\n---------- GRAPH STRUCTURE ----------\n";
    cout << "Total number of nodes: " << numNodes << endl;

    for (const auto &nodePair : adjacencyList)
    {
        int node = nodePair.first;
        cout << "Node " << node << " -> ";

        for (const Edge &edge : nodePair.second)
        {
            cout << edge.destination << " (weight: " << edge.weight << ") ";
        }

        cout << endl;
    }

    cout << "--------------------------------------\n\n";
}