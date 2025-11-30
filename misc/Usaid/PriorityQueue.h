#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <iostream>
#include <vector>
#include <stdexcept>
using namespace std;

template <typename T>
class PriorityQueue
{
    vector<T> heap;

    // Helper functions
    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return 2 * i + 1; }
    int rightChild(int i) { return 2 * i + 2; }

    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    PriorityQueue();

    void push(T val);
    void pop();
    T &top();
    int size() const;
    bool empty() const;
    void clear();

    // For debugging
    void printHeap() const;
};

template <typename T>
PriorityQueue<T>::PriorityQueue() {}

template <typename T>
void PriorityQueue<T>::heapifyUp(int index)
{
    while (index > 0 && heap[parent(index)] > heap[index])
    {
        swap(heap[index], heap[parent(index)]);
        index = parent(index);
    }
}

template <typename T>
void PriorityQueue<T>::heapifyDown(int index)
{
    int smallest = index;
    int left = leftChild(index);
    int right = rightChild(index);

    if (left < heap.size() && heap[left] < heap[smallest])
        smallest = left;

    if (right < heap.size() && heap[right] < heap[smallest])
        smallest = right;

    if (smallest != index)
    {
        swap(heap[index], heap[smallest]);
        heapifyDown(smallest);
    }
}

template <typename T>
void PriorityQueue<T>::push(T val)
{
    heap.push_back(val);
    heapifyUp(heap.size() - 1);
}

template <typename T>
void PriorityQueue<T>::pop()
{
    if (heap.empty())
    {
        throw underflow_error("PriorityQueue underflow: no elements to pop!");
    }

    heap[0] = heap.back();
    heap.pop_back();

    if (!heap.empty())
        heapifyDown(0);
}

template <typename T>
T &PriorityQueue<T>::top()
{
    if (heap.empty())
    {
        throw underflow_error("PriorityQueue is empty!");
    }

    return heap[0];
}

template <typename T>
int PriorityQueue<T>::size() const
{
    return heap.size();
}

template <typename T>
bool PriorityQueue<T>::empty() const
{
    return heap.empty();
}

template <typename T>
void PriorityQueue<T>::clear()
{
    heap.clear();
}

template <typename T>
void PriorityQueue<T>::printHeap() const
{
    cout << "Heap contents: ";
    for (const auto &val : heap)
        cout << val << " ";
    cout << endl;
}

#endif
