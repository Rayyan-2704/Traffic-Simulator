#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
#include <stdexcept>
using namespace std;

template <typename T>
class QueueListNode
{
public:
    T data;
    QueueListNode<T> *next;

    QueueListNode(T val) : data(val), next(nullptr) {}
};

template <typename T>
class Queue
{
    int Size;
    QueueListNode<T> *start;
    QueueListNode<T> *end;

public:
    Queue();

    void push(T val);
    void pop();
    T &front();
    int size() const;
    bool empty() const;

    ~Queue();
};

template <typename T>
Queue<T>::Queue() : Size(0), start(nullptr), end(nullptr) {}

template <typename T>
void Queue<T>::push(T val)
{
    QueueListNode<T> *newNode = new QueueListNode<T>(val);

    if (Size == 0)
    {
        start = newNode;
        end = newNode;
    }
    else
    {
        end->next = newNode;
        end = newNode;
    }

    Size++;
}

template <typename T>
void Queue<T>::pop()
{
    if (Size == 0)
    {
        throw underflow_error("Queue underflow: no elements to dequeue!");
    }

    QueueListNode<T> *temp = start;
    start = start->next;
    temp->next = nullptr;
    delete temp;
    Size--;

    if (Size == 0)
        end = nullptr;
}

template <typename T>
T &Queue<T>::front()
{
    if (start == nullptr)
    {
        throw underflow_error("Queue underflow: Queue is empty!");
    }

    return start->data;
}

template <typename T>
int Queue<T>::size() const
{
    return Size;
}

template <typename T>
bool Queue<T>::empty() const
{
    return start == nullptr;
}

template <typename T>
Queue<T>::~Queue()
{
    QueueListNode<T> *temp = start;
    while (temp != nullptr)
    {
        QueueListNode<T> *delNode = temp;
        temp = temp->next;
        delete delNode;
    }
}

#endif