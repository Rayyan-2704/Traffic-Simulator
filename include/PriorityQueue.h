#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include <utility>
#include "../include/Vehicle.h"
using namespace std;

/* Min-Heap Priority Queue for Dijkstra's Algorithm */
// Stores pairs of (distance, node) where smallest distance has highest priority
template <typename T>
class MinHeapPQ
{
    vector<T> heap;

    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return 2 * i + 1; }
    int rightChild(int i) { return 2 * i + 2; }

    bool hasHigherPriority(const T &a, const T &b)
    {
        // For min-heap: smaller value = higher priority
        return a.first < b.first;
    }

    void heapifyUp(int index)
    {
        while (index > 0 && hasHigherPriority(heap[index], heap[parent(index)]))
        {
            swap(heap[index], heap[parent(index)]);
            index = parent(index);
        }
    }

    void heapifyDown(int index)
    {
        int size = heap.size();
        int smallest = index;
        int left = leftChild(index);
        int right = rightChild(index);

        if (left < size && hasHigherPriority(heap[left], heap[smallest]))
            smallest = left;
        if (right < size && hasHigherPriority(heap[right], heap[smallest]))
            smallest = right;

        if (smallest != index)
        {
            swap(heap[index], heap[smallest]);
            heapifyDown(smallest);
        }
    }

public:
    MinHeapPQ() {}

    void push(const T &value)
    {
        heap.push_back(value);
        heapifyUp(heap.size() - 1);
    }

    T top() const
    {
        if (heap.empty())
            throw runtime_error("MinHeapPQ: Cannot call top() on empty queue!");
        return heap[0];
    }

    void pop()
    {
        if (heap.empty())
            throw runtime_error("MinHeapPQ: Cannot call pop() on empty queue!");

        heap[0] = heap.back();
        heap.pop_back();

        if (!heap.empty())
            heapifyDown(0);
    }

    bool empty() const { return heap.size() == 0; }
    int size() const { return heap.size(); }
    void clear() { heap.clear(); }

    // Print heap (for debugging)
    void printHeap() const
    {
        cout << "Heap contents (size: " << heap.size() << "): ";
        for (const auto &elem : heap)
        {
            cout << "(" << elem.first << "," << elem.second << ") ";
        }
        cout << endl;
    }
};

class MaxHeapPQ
{
    vector<Vehicle *> heap;

    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return 2 * i + 1; }
    int rightChild(int i) { return 2 * i + 2; }

    bool hasHigherPriority(Vehicle *a, Vehicle *b)
    {
        if (a == nullptr || b == nullptr)
            return false;

        // Higher priority number = more urgent = higher priority
        return a->getPriority() > b->getPriority();
    }

    void heapifyUp(int index)
    {
        while (index > 0 && hasHigherPriority(heap[index], heap[parent(index)]))
        {
            swap(heap[index], heap[parent(index)]);
            index = parent(index);
        }
    }

    void heapifyDown(int index)
    {
        int size = heap.size();
        int largest = index;
        int left = leftChild(index);
        int right = rightChild(index);

        if (left < size && hasHigherPriority(heap[left], heap[largest]))
            largest = left;

        if (right < size && hasHigherPriority(heap[right], heap[largest]))
            largest = right;

        if (largest != index)
        {
            swap(heap[index], heap[largest]);
            heapifyDown(largest);
        }
    }

public:
    MaxHeapPQ() {}

    void push(Vehicle *vehicle)
    {
        if (vehicle == nullptr)
        {
            cout << "Error: Cannot add null vehicle to heap!" << std::endl;
            return;
        }

        heap.push_back(vehicle);
        heapifyUp(heap.size() - 1);

        cout << "Vehicle " << vehicle->getID() << " (Priority: " << vehicle->getPriority() << ") added to heap" << endl;
    }

    Vehicle *top() const
    {
        if (heap.empty())
            throw runtime_error("MaxHeapPQ: Cannot call top() on empty queue!");
        return heap[0];
    }

    void pop()
    {
        if (heap.empty())
            throw runtime_error("MaxHeapPQ: Cannot call pop() on empty queue!");

        cout << "Removing Vehicle " << heap[0]->getID() << " (Priority: " << heap[0]->getPriority() << ")" << endl;

        heap[0] = heap.back();
        heap.pop_back();

        if (!heap.empty())
            heapifyDown(0);
    }

    bool empty() const { return heap.size() == 0; }
    int size() const { return heap.size(); }
    void clear() { heap.clear(); }

    // Print heap structure (for debugging)
    void printHeap() const
    {
        if (heap.empty())
        {
            cout << "Heap is empty" << endl;
            return;
        }

        cout << endl << "=== MAX HEAP STRUCTURE ===" << endl;
        cout << "Size: " << heap.size() << endl;
        cout << "Root (Highest Priority): Vehicle " << heap[0]->getID() << " (Priority: " << heap[0]->getPriority() << ")" << endl;

        cout << endl << "All vehicles in heap order:" << endl;
        for (int i = 0; i < heap.size(); i++)
        {
            cout << "  [" << i << "] Vehicle " << heap[i]->getID() << " (Priority: " << heap[i]->getPriority() << ")" << endl;
        }
        cout << "==========================\n"
             << endl;
    }
};

#endif