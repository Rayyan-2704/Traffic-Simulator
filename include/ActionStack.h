#ifndef ACTIONSTACK_H
#define ACTIONSTACK_H

#include "Stack.h"
#include <string>
#include "TrafficSignal.h"
using namespace std;

enum class ActionType
{
    TOGGLE_SIGNAL,
    SPAWN_VEHICLE
};

struct Action
{
    ActionType type;
    int targetID;              // Signal ID or Vehicle ID (depends on the action type)
    SignalState previousState; // For signal toggles
    SignalState newState;      // For signal toggles
    float timestamp;           // When action occurred

    // Constructor for signal actions
    Action(ActionType type, int id, SignalState prev, SignalState next, float time) : type(type), targetID(id), previousState(prev), newState(next), timestamp(time) {}

    // Constructor for non-signal actions
    Action(ActionType type, int id, float time) : type(type), targetID(id), previousState(SignalState::GREEN), newState(SignalState::GREEN), timestamp(time) {}
};

class ActionStack
{
private:
    Stack<Action> history;
    int maxSize;     // To limit stack size
    float totalTime; // To track simulation time

public:
    ActionStack(int maxCapacity = 100);

    // Core Stack Operations
    void push(const Action &action);
    Action pop();
    Action peek() const;

    // Undo functionality
    bool canUndo() const;
    Action undo(); // Pop and return last action
    void clear();  // Clear all history

    // Utility Functions
    int size() const;
    bool isEmpty() const;
    void setTime(float time);

    // For debugging
    void printLastAction() const;
};

#endif