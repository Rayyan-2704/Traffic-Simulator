#ifndef STACK_H
#define STACK_H

#include <iostream>
#include <stdexcept>
using namespace std;

template <typename T>
class StackNode
{
public:
    T data;
    StackNode<T> *next;

    StackNode(T val) : data(val), next(nullptr) {}
};

template <typename T>
class Stack
{
    int Size;
    StackNode<T> *top;

public:
    Stack();

    void push(T val);
    void pop();
    T &peek();
    int size() const;
    bool empty() const;

    ~Stack();
};

template <typename T>
Stack<T>::Stack() : Size(0), top(nullptr) {}

template <typename T>
void Stack<T>::push(T val)
{
    StackNode<T> *newNode = new StackNode<T>(val);
    newNode->next = top;
    top = newNode;
    Size++;
}

template <typename T>
void Stack<T>::pop()
{
    if (Size == 0)
    {
        throw underflow_error("Stack underflow: no elements to pop!");
    }

    StackNode<T> *temp = top;
    top = top->next;
    delete temp;
    Size--;
}

template <typename T>
T &Stack<T>::peek()
{
    if (top == nullptr)
    {
        throw underflow_error("Stack underflow: Stack is empty!");
    }

    return top->data;
}

template <typename T>
int Stack<T>::size() const
{
    return Size;
}

template <typename T>
bool Stack<T>::empty() const
{
    return top == nullptr;
}

template <typename T>
Stack<T>::~Stack()
{
    StackNode<T> *temp = top;
    while (temp != nullptr)
    {
        StackNode<T> *delNode = temp;
        temp = temp->next;
        delete delNode;
    }
}

#endif
