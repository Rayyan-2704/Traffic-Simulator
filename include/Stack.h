#ifndef STACK_H
#define STACK_H

#include <iostream>
#include <stdexcept>
using namespace std;

template <typename T>
class StackListNode
{
public:
    T data;
    StackListNode<T> *next;

    StackListNode(T val) : data(val), next(nullptr) {}
};

template <typename T>
class Stack
{
    int Size;
    StackListNode<T> *Top;

public:
    Stack();

    void push(T val);
    void pop();
    T &top() const;
    int size() const;
    bool empty() const;

    ~Stack();
};

template <typename T>
Stack<T>::Stack() : Size(0), Top(nullptr) {}

template <typename T>
void Stack<T>::push(T val)
{
    StackListNode<T> *newNode = new StackListNode<T>(val);
    newNode->next = Top;
    Top = newNode;
    Size++;
}

template <typename T>
void Stack<T>::pop()
{
    if (Top == nullptr)
    {
        throw underflow_error("Stack underflow: no elements to pop!");
    }

    StackListNode<T> *temp = Top;
    Top = Top->next;
    temp->next = nullptr;
    delete temp;
    Size--;
}

template <typename T>
T &Stack<T>::top() const
{
    if (Top == nullptr)
    {
        throw underflow_error("Stack underflow: stack is empty!");
    }

    return Top->data;
}

template <typename T>
int Stack<T>::size() const
{
    return Size;
}

template <typename T>
bool Stack<T>::empty() const
{
    return Size == 0;
}

template <typename T>
Stack<T>::~Stack()
{
    StackListNode<T> *temp = Top;
    while (temp != nullptr)
    {
        StackListNode<T> *delNode = temp;
        temp = temp->next;
        delete delNode;
    }
}

#endif